#include <math.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <pathfinding.h>
#include <spaces.h>

LOG_MODULE_REGISTER(pathfinding, LOG_LEVEL_INF);

/**
 * @brief Pathfinding workspace
 */
static uint8_t path_wspace[WORKSPACE_DIMENSION][WORKSPACE_DIMENSION];

/**
 * @brief Pathfinding configuration space
 */
static uint8_t path_cspace[CSPACE_DIMENSION][CSPACE_DIMENSION];

/**
 * @brief Mark the solution territory in cspace, given X,Y
 *
 * Will scan through cspace, calculating the endpoint
 */
static int mark_solution_region(int x, int y, int tolerance)
{
	int ret;

	double x_end;
	double y_end;

	for (int theta0 = 0; theta0 < ARM_RANGE; theta0 += ARM_DEGREE_INC) {
		for (int theta1 = 0; theta1 < ARM_RANGE; theta1 += ARM_DEGREE_INC) {
			ret = get_arm_endpoint(theta0, theta1, ARM_LEN_MM, ARM_RANGE, ARM_ORIGIN_X_MM, ARM_ORIGIN_Y_MM, &x_end, &y_end);
			if (ret) {
				LOG_ERR("Error calculating arm endpoint! (err: %d)", ret);
			}

			x_end = ceil(x_end);
			y_end = ceil(y_end);

			/*
			 * If cspace region is not obscured, mark it as potential solution space
			 */
			if (((int)x_end >= x - tolerance && (int)x_end <= x + tolerance) && ((int)y_end >= y - tolerance && (int)y_end <= y + tolerance)) {
				if (path_cspace[theta1][theta0] != 1) {
					path_cspace[theta1][theta0] = PATH;
				}
			}
		}
	}

	return 0;
}

int pathfinding_calculate_path(int start_theta0, int start_theta1, int end_x, int end_y, struct pathfinding_steps (*plan)[MAX_NUM_STEPS])
{
	/*
	 * 1. Reset path_wspace and path_cspace
	 */
	uint8_t (*temp_cspace)[CSPACE_DIMENSION];
	uint8_t (*temp_wspace)[WORKSPACE_DIMENSION];

	if (get_cspace(&temp_cspace)) {
		LOG_ERR("Couldn't get cspace!");
	}

	if (get_wspace(&temp_wspace)) {
		LOG_ERR("Couldn't get wspace!");
	}

	memcpy(path_cspace, temp_cspace, CSPACE_DIMENSION * sizeof(temp_cspace[0]));
	memcpy(path_wspace, temp_wspace, WORKSPACE_DIMENSION * sizeof(temp_wspace[0]));

	/*
	 * 2. Mark start/endpoint on wspace, if not occupied
	 */
	if (path_wspace[end_y][end_x] != 1) {
		path_wspace[end_y][end_x] = END_POINT;
	}
	/* TODO: Add a helper to map_utils that given theta0 and theta1, and the origin, calculates final X,Y coords */

	/*
	 * 3. Mark solution territory on cspace
	 */
	mark_solution_region(end_x, end_y, ALLOWABLE_TOLERANCE_MM);

	return 0;
}

int get_path_wspace(uint8_t (**path_wspace_out)[WORKSPACE_DIMENSION])
{
	if (path_wspace_out == NULL) {
		return -1;
	}

	*path_wspace_out = path_wspace;
	return 0;
}

int get_path_cspace(uint8_t (**path_cspace_out)[CSPACE_DIMENSION])
{
	if (path_cspace_out == NULL) {
		return -1;
	}

	*path_cspace_out = path_cspace;
	return 0;
}
