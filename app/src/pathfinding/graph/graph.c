/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <graph.h>

LOG_MODULE_REGISTER(graph, LOG_LEVEL_INF);

#ifdef CONFIG_GRAPH_BFS
/**
 * @brief Node struct used in graphing algorithm
 */
struct bfs_node {
	int x;
	int y;
};

/**
 * @brief Stack used for constructing parent path
 */
K_LIFO_DEFINE(stack);

/**
 * @brief Queue used for tracking neighbour we should search
 */
K_FIFO_DEFINE(queue);
static bool visited[CSPACE_DIMENSION][CSPACE_DIMENSION];
static struct bfs_node parent[CSPACE_DIMENSION][CSPACE_DIMENSION];

enum {
	UNVISITED = 0U,
	VISITED = 1U
};

/**
 * @brief based on the parent nodes, reconstruct the path back
 *
 * Essentially reversing the parent array to place into path array
 *
 * @retval 0 on success, -1 if path exceeds MAX_NUM_STEPS
 */
static int bfs_construct_path(int x, int y, int start_x, int start_y,
			      struct pathfinding_steps path[MAX_NUM_STEPS], int *num_steps)
{
	int ret;
	int count = 0;

	LOG_INF("Reconstructing BFS path... from x: %d y: %d back to start_x: %d start_y :%d", x, y,
		start_x, start_y);

	/* Put end node onto stack */
	struct bfs_node end_node = {.x = x, .y = y};

	ret = k_lifo_alloc_put(&stack, &end_node);
	if (ret) {
		LOG_ERR("ERROR lifo alloc failed!");
	}
	count++;

	/* Push the solution path to stack */
	while (true) {
		if (count >= MAX_NUM_STEPS) {
			LOG_ERR("ERROR Solution path exceeds MAX_NUM_STEPS %d", MAX_NUM_STEPS);
			return -1;
		}

		if (parent[y][x].x == start_x && parent[y][x].y == start_y) {
			*num_steps = count;
			break;
		}

		if (x < 0 || x >= CSPACE_DIMENSION || y < 0 || y >= CSPACE_DIMENSION) {
			LOG_ERR("ERROR X or Y in reverse path out of range!");
			return -1;
		}

		ret = k_lifo_alloc_put(&stack, &parent[y][x]);
		if (ret) {
			LOG_ERR("ERROR lifo alloc failed!");
		}

		x = parent[y][x].x;
		y = parent[y][x].y;
		count++;
	}

	/* Pop stack into solution array */
	int i = 0;
	struct bfs_node *data;
	while ((data = k_lifo_get(&stack, K_NO_WAIT)) != NULL) {
		path[i].theta0 = data->x;
		path[i].theta1 = data->y;
		i++;
	}

	/* Free all remaining entries in queue and stack */
	while ((data = k_fifo_get(&queue, K_NO_WAIT)) != NULL) {
		data->x = -1;
		data->y = -1;
		k_free(data);
	}

	LOG_INF("Finished calculating path to end");

	return 0;
}

static void add_neighbour(int x, int y, struct bfs_node *parent_node,
			  uint8_t (*graph)[CSPACE_DIMENSION])
{
	int ret;

	if (x < 0 || x >= CSPACE_DIMENSION || y < 0 || y >= CSPACE_DIMENSION) {
		return;
	}

	if (visited[y][x] != VISITED && graph[y][x] != OCCUPIED) {

		/* Create graph node */
		struct bfs_node *node = k_malloc(sizeof(struct bfs_node));
		if (node == NULL) {
			LOG_ERR("ERROR insufficient memory to alloc node!");
		}
		node->x = x;
		node->y = y;

		/* Push neighbour to queue */
		ret = k_fifo_alloc_put(&queue, node);
		if (ret) {
			LOG_ERR("ERROR during fifo alloc!");
		}

		/* Mark neighbour node as visited */
		visited[y][x] = VISITED;

		/* Perform memcpy of node to parent array */
		memcpy(&parent[y][x], parent_node, sizeof(struct bfs_node));
	}
}

static int bfs(uint8_t (*graph)[CSPACE_DIMENSION], int start_x, int start_y,
	       struct pathfinding_steps path[MAX_NUM_STEPS], int *num_steps)
{
	int ret;

	/* Reset queue, and metadata arrays */
	struct bfs_node *data;
	while ((data = k_fifo_get(&queue, K_NO_WAIT)) != NULL) {
		continue;
	}
	memset(visited, 0, sizeof(bool) * CSPACE_DIMENSION * CSPACE_DIMENSION);
	memset(parent, 0, sizeof(struct bfs_node) * CSPACE_DIMENSION * CSPACE_DIMENSION);

	struct bfs_node *start = k_malloc(sizeof(struct bfs_node));
	if (start == NULL) {
		LOG_ERR("ERROR insufficient memory to alloc start node!");
	}
	start->x = start_x;
	start->y = start_y;

	ret = k_fifo_alloc_put(&queue, start);
	if (ret) {
		LOG_ERR("ERROR during fifo alloc!");
	}

	struct bfs_node *node;
	while ((node = (struct bfs_node *)k_fifo_get(&queue, K_NO_WAIT)) != NULL) {

		if (graph[node->y][node->x] == END_POINT) {
			LOG_INF("BFS found path to solution: %d\u00B0, %d\u00B0", node->x, node->y);
			return bfs_construct_path(node->x, node->y, start_x, start_y, path,
						  num_steps);
		}

		/* For each neighbour, see if we need to visit them */
		add_neighbour(node->x + 1, node->y, node, graph);
		add_neighbour(node->x - 1, node->y, node, graph);
		add_neighbour(node->x, node->y + 1, node, graph);
		add_neighbour(node->x, node->y - 1, node, graph);
		add_neighbour(node->x - 1, node->y + 1, node, graph);
		add_neighbour(node->x + 1, node->y + 1, node, graph);
		add_neighbour(node->x - 1, node->y - 1, node, graph);
		add_neighbour(node->x + 1, node->y - 1, node, graph);

		/* Free node before continuing */
		node->x = -1;
		node->y = -1;
		k_free(node);
	}

	/* No path found! */
	LOG_ERR("ERROR No path found!");
	return -1;
}
#endif

int graph_path(uint8_t (*graph)[CSPACE_DIMENSION], int start_x, int start_y,
	       struct pathfinding_steps path[MAX_NUM_STEPS], int *num_steps)
{
	LOG_INF("Graphing path from %d\u00B0, %d\u00B0", start_x, start_y);

#ifdef CONFIG_GRAPH_BFS
	return bfs(graph, start_x, start_y, path, num_steps);
#endif

	LOG_ERR("No graphing algorithm chosen in Kconfig!");
	return -1;
}
