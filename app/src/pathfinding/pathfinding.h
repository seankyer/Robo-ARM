/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_PATHFINDING_H_
#define APP_PATHFINDING_H_

#include <common.h>
#include <spaces.h>

/**
 * @brief Max number of steps in the manipulation
 */
#define MAX_NUM_STEPS 300

/**
 * @brief Arbitrarily pick solution nodes for heuristic
 */
#define SOLUTION_NODES 5

/**
 * @brief Struct holding a single step of path
 */
struct pathfinding_steps {
	int theta0; /**< Angle of inclination for ARM0 */
	int theta1; /**< Angle of inclination for ARM1 */
};

/**
 * @brief Calculate the path from a given angle state to end X,Y point in workspace
 *
 * This function, given the currently computed cspace and wspace, will run a
 * pathfinding algorithm calculating a series of angle steps from start_theta0
 * and start__theta1 to the desired X,Y coordinates.
 *
 * This function stores a copy of the wspace and cspace and will annotate
 * the path taken on each. Using the get_pathfind_cspace and get_pathfind_wspace
 * functions allow you to retrieve a copy of the routing algorithms work.
 *
 * @param[in] start_theta0 The origin theta0 in cspace
 * @param[in] start_theta1 The origin theta1 in cspace
 * @param[in] end_x The target X coordinate in workspace
 * @param[in] end_y The target Y coordinate in workspace
 * @param[out] plan Array of pathfinding steps to get from start to end
 *
 * @retval 0 on success, non-zero otherwise
 */
int pathfinding_calculate_path(int start_theta0, int start_theta1, int end_x, int end_y,
			       struct pathfinding_steps plan[MAX_NUM_STEPS], int *num_steps);

#endif /* APP_PATHFINDING_H_ */
