#include <math.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <pathfinding.h>
#include <spaces.h>

LOG_MODULE_REGISTER(pathfinding, LOG_LEVEL_INF);

/**
 * @brief Endpoint marker in wspace
 */
#define ENDPOINT_MARK 2

/**
 * @brief Startpoint marker in wspace
 */
#define STARTPOINT_MARK 3

/**
 * @brief Solution space marker
 */
#define SOLUTION_MARK 2

/**
 * @brief Allowable tolerance for pathfinding
 *
 * It's unlikely that a combination of angles will exactly equal
 * the target X,Y coordinate, so we must define some allowable
 * error tolerance in mm
 */
#define ALLOWABLE_TOLERANCE_MM 1

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

	for (int theta0 = 0; theta0 < ARM_RANGE; theta0 += ARM_DEGREE_INC) {

		double x0_delta;
		double y0_delta;

		/*
		 * This gets the endpoint assuming origin of 0
		 */
		ret = get_segment_endpoint_trig(ARM_LEN_MM, (double)theta0, &x0_delta, &y0_delta);
		if (ret) {
			LOG_ERR("Error during segment endpoint calculation (err: %d)\n", ret);
			return ret;
		}

		double x0_endpoint = ARM_ORIGIN_X_MM + x0_delta;
		double y0_endpoint = ARM_ORIGIN_Y_MM + y0_delta;

		for (int theta1 = 0; theta1 < ARM_RANGE; theta1 += ARM_DEGREE_INC) {

			double x1_delta;
			double y1_delta;

			ret = get_segment_endpoint_trig(ARM_LEN_MM,
							(double)theta1 + (theta0 - (ARM_RANGE / 2)),
							&x1_delta, &y1_delta);
			if (ret) {
				LOG_ERR("Error during segment endpoint calculation (err: %d)\n",
					ret);
				return ret;
			}

			int x1_endpoint = ceil(x0_endpoint + x1_delta);
			int y1_endpoint = ceil(y0_endpoint + y1_delta);

			/*
			 * If cspace region is not obscured, mark it as potential solution space
			 */
			if ((x1_endpoint >= x - tolerance && x1_endpoint <= x + tolerance) && (y1_endpoint >= y - tolerance && y1_endpoint <= y + tolerance)) {
				if (path_cspace[theta1][theta0] != 1) {
					path_cspace[theta1][theta0] = SOLUTION_MARK;
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
	 * 2. Mark start/endpoint on wspace
	 */
	path_wspace[end_y][end_x] = ENDPOINT_MARK;
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
