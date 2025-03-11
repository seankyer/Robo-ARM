/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <math.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <lib/map_utils.h>
#include <pathfinding.h>
#include <spaces.h>
#include <graph.h>

LOG_MODULE_REGISTER(pathfinding, LOG_LEVEL_INF);

/**
 * @brief Pathfinding workspace
 */
static uint8_t (*path_wspace)[WORKSPACE_DIMENSION];

/**
 * @brief Pathfinding configuration space
 */
static uint8_t (*path_cspace)[CSPACE_DIMENSION];

/**
 * @brief Using routing algorithm, calculate efficient solution to cspace solution space
 *
 * Assumes that path_cspace and path_wspace contain valid data
 */
static int calculate_path(struct pathfinding_steps plan[MAX_NUM_STEPS], int *num_steps,
			  int start_theta0, int start_theta1,
			  struct point solutions[SOLUTION_NODES])
{
	return graph_path(path_cspace, start_theta0, start_theta1, plan, num_steps, solutions);
}

/**
 * @brief Mark the solution territory in cspace, given X,Y
 *
 * Will scan through cspace, calculating the endpoint
 */
static int mark_solution_region(int x, int y, int tolerance, struct point solutions[SOLUTION_NODES])
{
	int ret;

	/*
	 * If solution not found, report error
	 */
	bool solution = false;
	int idx = 0;

	double x_end;
	double y_end;

	for (int theta0 = 0; theta0 < ARM_RANGE; theta0 += ARM_DEGREE_INC) {
		for (int theta1 = 0; theta1 < ARM_RANGE; theta1 += ARM_DEGREE_INC) {
			ret = get_arm_endpoint(theta0, theta1, ARM_LEN_MM, ARM_RANGE,
					       ARM_ORIGIN_X_MM, ARM_ORIGIN_Y_MM, &x_end, &y_end);
			if (ret) {
				LOG_ERR("Error calculating arm endpoint! (err: %d)", ret);
				return ret;
			}

			x_end = ceil(x_end);
			y_end = ceil(y_end);

			/*
			 * If cspace region is not obscured, mark it as potential solution space
			 */
			if (((int)x_end >= x - tolerance && (int)x_end <= x + tolerance) &&
			    ((int)y_end >= y - tolerance && (int)y_end <= y + tolerance)) {
				if (path_cspace[theta1][theta0] != OCCUPIED) {
					path_cspace[theta1][theta0] = END_POINT;
					solution = true;

					/* TODO: Replace this with a distributed approach */
					if (idx < SOLUTION_NODES) {
						solutions[idx].x = theta0;
						solutions[idx].y = theta1;
						idx++;
					} else {
						LOG_INF("Max solution nodes hit");
					}
				}
			}
		}
	}

	if (!solution) {
		LOG_ERR("ERROR Solution to desired coordinates not found!");
		return -1;
	}

	return 0;
}

int pathfinding_calculate_path(int start_theta0, int start_theta1, int end_x, int end_y,
			       struct pathfinding_steps plan[MAX_NUM_STEPS], int *num_steps)
{
	int ret;

	LOG_INF("Pathfinding is calculating path to: X: %dmm, Y: %dmm from angles theta0: "
		"%d\u00B0, theta1: %d\u00B0",
		end_x, end_y, start_theta0, start_theta1);

	/*
	 * Check for proper inputs
	 */
	if (start_theta0 >= ARM_RANGE || start_theta0 < 0 || start_theta1 >= ARM_RANGE ||
	    start_theta1 < 0 || end_x < 0 || end_x >= WORKSPACE_DIMENSION || end_y < 0 ||
	    end_y >= WORKSPACE_DIMENSION) {
		LOG_ERR("ERROR: Parameters supplied invalid!");
		return -EINVAL;
	}

	/*
	 * 1. Copy over pointers to spaces
	 */
	path_cspace = get_cspace();
	path_wspace = get_wspace();

	/*
	 * 2. Mark start/endpoint in spaces, if not occupied
	 */
	if (path_cspace[start_theta1][start_theta0] != FREE) {
		LOG_ERR("ERROR Provided starting angles are in invalid configuration!");
		return -EINVAL;
	}

	path_cspace[start_theta1][start_theta0] = START_POINT;

	if (path_wspace[end_y][end_x] != FREE) {
		LOG_ERR("ERROR: End coordinates supplied are in occupied space!");
		return -1;
	}

	path_wspace[end_y][end_x] = END_POINT;

	double temp_x;
	double temp_y;
	ret = get_arm_endpoint(start_theta0, start_theta1, ARM_LEN_MM, ARM_RANGE, ARM_ORIGIN_X_MM,
			       ARM_ORIGIN_Y_MM, &temp_x, &temp_y);
	if (ret) {
		LOG_ERR("ERROR calculating arm endpoint (err: %d)", ret);
		return ret;
	}

	temp_x = ceil(temp_x);
	temp_y = ceil(temp_y);

	/*
	 * Check the starting X,Y coordinates are legal
	 */
	if ((int)temp_x < 0 || (int)temp_x >= WORKSPACE_DIMENSION || (int)temp_y < 0 ||
	    (int)temp_x >= WORKSPACE_DIMENSION) {
		LOG_ERR("ERROR: Starting points, given angles, out of wspace range! (x: %d, y: %d)",
			(int)temp_x, (int)temp_y);
		return -1;
	}

	if (path_wspace[(int)temp_y][(int)temp_x] != FREE) {
		LOG_ERR("ERROR: Starting coordinates, given angles, are in occupied space! (x: %d, "
			"y: %d)",
			(int)temp_y, (int)temp_x);
		return -1;
	}

	path_wspace[(int)temp_y][(int)temp_x] = START_POINT;

	struct point solutions[SOLUTION_NODES];

	/*
	 * 3. Mark solution territory on cspace
	 */
	ret = mark_solution_region(end_x, end_y, ALLOWABLE_TOLERANCE_MM, solutions);
	if (ret) {
		LOG_ERR("ERROR marking solution region! (err: %d)", ret);
		return ret;
	}

	LOG_INF("Solution region marked");

	LOG_INF("Calculating path to solution");

	ret = calculate_path(plan, num_steps, start_theta0, start_theta1, solutions);
	if (ret) {
		LOG_ERR("ERROR calculating solution path! (err: %d)", ret);
		return ret;
	}

	/*
	 * 4. Draw solution on cspace
	 *
	 * Don't draw last point to keep end-point marker
	 */
	for (int i = 1; i < *num_steps - 1; i++) {
		path_cspace[plan[i].theta1][plan[i].theta0] = PATH;

		double x;
		double y;
		ret = get_arm_endpoint(plan[i].theta0, plan[i].theta1, ARM_LEN_MM, ARM_RANGE,
				       ARM_ORIGIN_X_MM, ARM_ORIGIN_Y_MM, &x, &y);
		if (ret) {
			LOG_ERR("Error calculating arm endpoint! (err: %d)", ret);
			return ret;
		}

		path_wspace[(int)ceil(y)][(int)ceil(x)] = PATH;
	}

	LOG_INF("Pathfinding completed successfully, solution staged!");

	return 0;
}
