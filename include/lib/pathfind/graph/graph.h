/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_GRAPH_H_
#define APP_GRAPH_H_

#include <lib/pathfind/pathfinding.h>

/**
 * @brief struct to represent a point on graph
 */
struct point {
	uint16_t x; /**< X coordinate */
	uint16_t y; /**< Y coordinate */
};

/**
 * @brief Run the pathfinding algorithm on the supplied graph
 *
 * @param[in] graph Pointer to graph
 * @param[in] start_x Starting X coordinate on graph
 * @param[in] start_y Starting Y coordinate on graph
 * @param[in] sol_mark Marker on graph denoting solution square
 * @param[out] path Pointer to solution path
 * @param[out] num_steps Length of path
 *
 * @retval 0 on success, non-zero otherwise
 */
int graph_path(const uint8_t (*graph)[CSPACE_DIMENSION], const int start_x, const int start_y,
	       struct pathfinding_steps path[MAX_NUM_STEPS], int *num_steps,
	       struct point end_points[SOLUTION_NODES]);

#endif /* APP_GRAPH_H_ */
