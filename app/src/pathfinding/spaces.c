/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <spaces.h>

LOG_MODULE_REGISTER(spaces, LOG_LEVEL_INF);

/**
 * @brief Upper limit for number of obstacles
 */
#define MAX_NUM_OBJ 10

/**
 * @brief Number of known obstacles
 */
static int num_obstacles;

/**
 * @brief List of obstacles in workspace
 */
static struct rectangle obstacles[MAX_NUM_OBJ];

/**
 * @brief Workspace array
 */
static uint8_t wspace[WORKSPACE_DIMENSION][WORKSPACE_DIMENSION] = {{FREE}};

/**
 * @brief Configuration space array
 *
 * ARM_RANGE x ARM_RANGE grid of possible configurations for arms
 */
static uint8_t cspace[CSPACE_DIMENSION][CSPACE_DIMENSION] = {{FREE}};

/**
 * @brief Checks if arm position collides with environment
 *
 * Verifies against the workspace if the coordinates from global
 * origin to specified coordinates intersect with any obstacles
 * in the workspace.
 *
 * @param[in] x x coordinate of endpoint of arm
 * @param[in] y y coordinate of endpoint of arm
 *
 * @retval False if no collisions
 * @retval True is collision
 *
 */
static bool check_collisions(double orig_x, double orig_y, double end_x, double end_y)
{
	LOG_DBG("Checking collisions for segment spanning from (%f, %f) to (%f, %f)", orig_x,
		orig_y, end_x, end_y);

	struct segment seg = {.x1 = orig_x, .y1 = orig_y, .x2 = end_x, .y2 = end_y};

	/* Iterate through each known object */
	for (int i = 0; i < num_obstacles; i++) {

		/* Translate by the the thickness of our plus clearance arm */
		for (int mag = -(ARM_WIDTH_MM / 2) - REQUIRED_CLEARANCE_MM;
		     mag <= (ARM_WIDTH_MM / 2) + REQUIRED_CLEARANCE_MM;
		     mag += (ARM_WIDTH_MM + (REQUIRED_CLEARANCE_MM * 2)) / 4) {

			/* Instantly return if a collision is found anywhere along arm width or
			 * clearance */
			if (check_segment_rectangle_collisions(translate_segment(seg, mag),
							       obstacles[i])) {
				return true;
			}
		}
	}

	return false;
}

/**
 * @brief Marks the rectangle in the workspace as occupied
 *
 * @param[in] obstacle The obstacle to be marked
 */
static void mark_obstacle_in_workspace(const struct rectangle *obstacle)
{
	int min_x = (int)fmin(fmin(fmin(obstacle->bottom.x1, obstacle->bottom.x2),
				   fmin(obstacle->top.x1, obstacle->top.x2)),
			      fmin(fmin(obstacle->left.x1, obstacle->left.x2),
				   fmin(obstacle->right.x1, obstacle->right.x2)));
	int max_x = (int)fmax(fmax(fmax(obstacle->bottom.x1, obstacle->bottom.x2),
				   fmax(obstacle->top.x1, obstacle->top.x2)),
			      fmax(fmax(obstacle->left.x1, obstacle->left.x2),
				   fmax(obstacle->right.x1, obstacle->right.x2)));
	int min_y = (int)fmin(fmin(fmin(obstacle->bottom.y1, obstacle->bottom.y2),
				   fmin(obstacle->top.y1, obstacle->top.y2)),
			      fmin(fmin(obstacle->left.y1, obstacle->left.y2),
				   fmin(obstacle->right.y1, obstacle->right.y2)));
	int max_y = (int)fmax(fmax(fmax(obstacle->bottom.y1, obstacle->bottom.y2),
				   fmax(obstacle->top.y1, obstacle->top.y2)),
			      fmax(fmax(obstacle->left.y1, obstacle->left.y2),
				   fmax(obstacle->right.y1, obstacle->right.y2)));

	/* Assumption: That rectangle is axis aligned allows us to do this */
	for (int y = min_y; y <= max_y; y++) {
		for (int x = min_x; x <= max_x; x++) {
			if (x >= 0 && x < WORKSPACE_SQMM && y >= 0 && y < WORKSPACE_SQMM) {
				wspace[y][x] = OCCUPIED;
			}
		}
	}
}

int add_obstacle(const struct rectangle *obstacle)
{
	if (num_obstacles >= MAX_NUM_OBJ) {
		return -1;
	}

	mark_obstacle_in_workspace(obstacle);

	obstacles[num_obstacles] = *obstacle;
	num_obstacles++;

	return 0;
}

int generate_configuration_space()
{
	LOG_INF("Generating Configuration Space!!!");

	int ret;

	/*
	 * Run through the entire range of motion for the 2-axis arm
	 * and record if its a valid placement or not to the configuration space
	 */
	for (int theta0 = 0; theta0 < ARM_RANGE; theta0 += ARM_DEGREE_INC) {

		LOG_INF("Generating... %d%% complete", (theta0 * 100) / 180);

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

		/*
		 * Calculate collisions in workspace
		 */
		if (check_collisions(ARM_ORIGIN_X_MM, ARM_ORIGIN_Y_MM, x0_endpoint, y0_endpoint)) {
			for (int j = 0; j < ARM_RANGE; j += ARM_DEGREE_INC) {
				LOG_DBG("Recording collision at angles: (theta0: %d, theta1: %d)",
					theta0, j);
				cspace[j][theta0] = OCCUPIED;
			}
			continue;
		}

		/*
		 * Our model does not rotate the axis, instead assume theta1 = 0 is
		 * perpendicular to theta0 since each arm is in series with each other.
		 */
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

			double x1_endpoint = x0_endpoint + x1_delta;
			double y1_endpoint = y0_endpoint + y1_delta;

			/*
			 * Calculate collisions in workspace
			 */
			if (check_collisions(x0_endpoint, y0_endpoint, x1_endpoint, y1_endpoint)) {
				LOG_DBG("Recording collision at angles: (theta0: %d, theta1: %d)",
					theta0, theta1);
				cspace[theta1][theta0] = OCCUPIED;
				continue;
			}

			/* Calculate if arm is in-bounds. Mark as occupied if not. */
			double temp_x;
			double temp_y;
			ret = get_arm_endpoint(theta0, theta1, ARM_LEN_MM, ARM_RANGE,
					       ARM_ORIGIN_X_MM, ARM_ORIGIN_Y_MM, &temp_x, &temp_y);
			if (ret) {
				LOG_ERR("ERROR calculating arm endpoint (err: %d)", ret);
				return ret;
			}

			int int_x = (int)ceil(temp_x);
			int int_y = (int)ceil(temp_y);
			if (int_x < 0 || int_x >= WORKSPACE_DIMENSION || int_y < 0 ||
			    int_y >= WORKSPACE_DIMENSION) {
				cspace[theta1][theta0] = OCCUPIED;
			}
		}
	}

	LOG_INF("Finished Generating Configuration Space");

	return 0;
}

uint8_t (*get_wspace(void))[WORKSPACE_DIMENSION]
{
	return wspace;
}

uint8_t (*get_cspace(void))[CSPACE_DIMENSION]
{
	return cspace;
}
