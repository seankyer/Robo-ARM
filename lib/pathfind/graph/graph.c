/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <math.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <lib/pathfind/graph/graph.h>

LOG_MODULE_REGISTER(graph, LOG_LEVEL_INF);

/**
 * @brief Heap used for graphing
 */
K_HEAP_DEFINE(heap, 16384);

/**
 * @brief Node struct used in graphing algorithm
 */
struct node {
	struct point pos;    /**< Node position on graph */
	uint16_t distance;   /**< Distance from an end-point */
	struct node *parent; /**< Pointer to parent node */
	struct node *next;   /**< Pointer to next node in linked-list */
};

/**
 * @brief One bit per cell in graph
 */
#define BITMAP_SIZE ((CSPACE_DIMENSION * CSPACE_DIMENSION) / 8)

/**
 * @brief Visited array
 */
static uint8_t visited[BITMAP_SIZE] = {0};

/**
 * @brief Helper function for bitmap
 *
 * @param[in] x X coordinate
 * @param[in] y Y coordinate
 *
 * @retval Index of bit position
 */
static inline int get_bit_index(int x, int y)
{
	return y * CSPACE_DIMENSION + x;
}

/**
 * @brief Mark the node as visited
 *
 * @param[in] x X coordinate
 * @param[in] y Y coordinate
 */
static void mark_visited(int x, int y)
{
	int index = get_bit_index(x, y);
	visited[index / 8] |= (1 << (index % 8));
}

/**
 * @brief Check if a node has been visited
 *
 * @param[in] x X coordinate
 * @param[in] y Y coordinate
 *
 * @retval True if node visited, False otherwise
 */
static bool is_visited(int x, int y)
{
	int index = get_bit_index(x, y);
	return (visited[index / 8] & (1 << (index % 8))) != 0;
}

/**
 * @brief Checks if node is valid
 *
 * Checks that the position of the point is within bounds of graph,
 * the position on the graph is not occupied and we haven't visited
 * this node before.
 *
 * @param[in] pos Point to check
 * @param[in] graph Pointer to graph
 *
 * @retval True if valid, False otherwise
 */
static inline bool is_valid(struct point pos, const uint8_t (*graph)[CSPACE_DIMENSION])
{
	return (pos.x > 0 && pos.x < CSPACE_DIMENSION && pos.y > 0 && pos.y < CSPACE_DIMENSION &&
		graph[pos.y][pos.x] != OCCUPIED && !is_visited(pos.x, pos.y));
}

/**
 * @brief Returns the distance to the closest end_point
 *
 * Does not take into account obstacles on your way
 *
 * @param[in] curr_x X coordinate of current node
 * @param[in] curr_y Y coordinate of current node
 * @param[in] end_points Array of potential solution end points
 *
 * @retval Euclidean distance to closest end-point in absolute terms
 */
static int calculate_distance(double curr_x, double curr_y, struct point end_points[SOLUTION_NODES])
{
	int min = INT32_MAX;
	for (int i = 0; i < SOLUTION_NODES; i++) {
		int temp;
		temp = (int)ceil(sqrt(
			(curr_x - (double)end_points[i].x) * (curr_x - (double)end_points[i].x) +
			(curr_y - (double)end_points[i].y) * (curr_y - (double)end_points[i].y)));
		if (temp < min) {
			min = temp;
		}
	}
	return min;
}

/**
 * @brief Helper to generate possible neighbours
 *
 * @param[in] pos Point to generate neighbours from
 * @param[out] neighbours Generated neighbours in all directions
 */
static void get_neighbours(struct point pos, struct point neighbours[8])
{
	neighbours[0] = (struct point){pos.x, pos.y + 1};
	neighbours[1] = (struct point){pos.x, pos.y - 1};
	neighbours[2] = (struct point){pos.x + 1, pos.y};
	neighbours[3] = (struct point){pos.x - 1, pos.y};
	neighbours[4] = (struct point){pos.x + 1, pos.y + 1};
	neighbours[5] = (struct point){pos.x - 1, pos.y + 1};
	neighbours[6] = (struct point){pos.x - 1, pos.y - 1};
	neighbours[7] = (struct point){pos.x + 1, pos.y - 1};
}

/**
 * @brief Helper to add a node to linked-list preserving min-distance order
 *
 * @param[in] head Pointer to head of linked list
 * @param[in] new_node Pointer to new node to add
 *
 * @retval The pointer to the head of the list
 */
static struct node *add_ordered(struct node *head, struct node *new_node)
{
	struct node *prev = NULL;
	struct node *next = head;

	/* Base case, empty head given, new_node is now head */
	if (!head) {
		return new_node;
	}

	/* Base case, new_node is now the shortest distance */
	if (new_node->distance < head->distance) {
		new_node->next = head;
		return new_node;
	}

	/* Scan for where new_node slots in */
	while (new_node->distance >= next->distance) {
		prev = next;
		next = next->next;

		/* End of list! */
		if (next == NULL) {
			break;
		}
	}

	/* Insert node at position */
	new_node->next = next;
	prev->next = new_node;

	/* Return the original head */
	return head;
}

/**
 * @brief Inserts a node into the linked-list
 *
 * @param[in] head Pointer to head of linked-list
 * @param[in] curr Pointer of the node spawning this action
 * @param[in] end_points Array of potential solutions
 * @param[in] graph Pointer to graph array
 *
 * @retval New head of linked-list
 */
static struct node *add_boundary(struct node *head, struct node *curr,
				 struct point end_points[SOLUTION_NODES],
				 const uint8_t (*graph)[CSPACE_DIMENSION])
{
	struct point neighbours[8];

	/* Generate all possible positions */
	get_neighbours(curr->pos, neighbours);

	/* Iterate through each neighbour */
	for (int i = 0; i < 8; i++) {
		struct point new_point = {.x = neighbours[i].x, .y = neighbours[i].y};

		/* Skip invalid suggestions */
		if (!is_valid(new_point, graph)) {
			continue;
		}

		/* Mark node as visited to prevent cycles */
		mark_visited(new_point.x, new_point.y);

		/* Skip entries that increase our distance */
		int distance = calculate_distance(new_point.x, new_point.y, end_points);

		/* Create new node and fill pointer */
		struct node *new = k_heap_alloc(&heap, sizeof(struct node), K_NO_WAIT);
		if (!new) {
			LOG_ERR("ERROR Out of memory!");
		}

		new->pos = new_point;
		new->distance = distance;
		new->parent = curr;
		head = add_ordered(head, new);
	}

	return head;
}

/**
 * @brief Greedy variant of dijkstra pathfinding algorithm
 *
 * Greedy variant gives us a nice mix between an optimal solution,
 * without explosive memory consumption when there are many dead-ends
 * as the greedy approach via distance calculation keeps us heading in
 * the correct direction most times.
 *
 * @param[in] graph Pointer to graph to perform pathfind on
 * @param[in] start_x Starting X coordinate on graph
 * @param[in] start_y Starting Y coordinate on graph
 * @param[in] end_points Array of potential solution points
 * @param[out] path Pointer to hold found steps to solution
 * @param[out] num_steps Number of steps on path
 *
 * @retval 0 on success, non-zero otherwise
 */
static int greedy_dijkstra(const uint8_t (*graph)[CSPACE_DIMENSION], const int start_x,
			   const int start_y, struct point end_points[SOLUTION_NODES],
			   struct pathfinding_steps path[MAX_NUM_STEPS], int *num_steps)
{
	LOG_INF("Starting traversal of graph");

	/* Initialize data structures */
	struct node *head = k_heap_alloc(&heap, sizeof(struct node), K_NO_WAIT);
	if (!head) {
		LOG_ERR("ERROR Out of memory!");
		return -ENOMEM;
	}

	struct point start_p = {.x = start_x, .y = start_y};

	head->pos = start_p;
	head->parent = NULL;
	head->next = NULL;
	head->distance = calculate_distance(head->pos.x, head->pos.y, end_points);

	struct node *curr = head;
	while (curr) {

		/* Pop head from linked list */
		head = head->next;

		/* Check if solution found */
		if (graph[curr->pos.y][curr->pos.x] == END_POINT) {
			break;
		}

		/* Increase your boundary */
		head = add_boundary(head, curr, end_points, graph);

		/* If add_boundary couldn't add any more nodes, this will equate to NULL and loop
		 * terminates */
		curr = head;
	}

	if (!curr) {
		LOG_ERR("ERROR No path found!");
		return -1;
	}

	LOG_INF("Path found!");

	/* Save our solution node */
	struct node *final = curr;

	/* Reconstruct path */
	int count = 1;
	while (curr->pos.x != start_x || curr->pos.y != start_y) {
		curr = curr->parent;
		if (count++ >= MAX_NUM_STEPS) {
			LOG_ERR("Path to solution exceeds MAX_NUM_STEPS: %d", MAX_NUM_STEPS);
			break;
		}
	}

	curr = final;
	*num_steps = count;
	struct node *temp;
	for (int i = count - 1; i >= 0; i--) {
		path[i].theta0 = curr->pos.x;
		path[i].theta1 = curr->pos.y;
		temp = curr;
		curr = curr->parent;
		k_heap_free(&heap, temp);
	}

	/* Free all remaining memory stuck in linked list */
	while (head) {
		temp = head;
		head = head->next;
		k_heap_free(&heap, temp);
	}

	LOG_INF("Done calculating path to solution");
	return 0;
}

int graph_path(const uint8_t (*graph)[CSPACE_DIMENSION], const int start_x, const int start_y,
	       struct pathfinding_steps path[MAX_NUM_STEPS], int *num_steps,
	       struct point end_points[SOLUTION_NODES])
{
	LOG_INF("Graphing path from %d\u00B0, %d\u00B0", start_x, start_y);
	return greedy_dijkstra(graph, start_x, start_y, end_points, path, num_steps);
}
