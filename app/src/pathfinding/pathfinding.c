#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <pathfinding.h>

LOG_MODULE_REGISTER(pathfinding, LOG_LEVEL_INF);

/* Configuration space representation */
#define ARM_LEN_MM     150 /* arm length in mm */
#define ARM_WIDTH_MM   30  /* arm length in mm */
#define ARM_RANGE      180 /* arm range of motion in degrees */
#define ARM_DEGREE_INC 1   /* degrees increment */

/* Real-world 'workspace' representation */
#define WORKSPACE_SQMM 395

/**
 * @brief Required mm of clearance between arm edge and obstacles
 */
#define REQUIRED_CLEARANCE_MM 3

/**
 * @brief Arm X origin
 */
#define ARM_ORIGIN_X_MM WORKSPACE_SQMM / 2

/**
 * @brief Arm Y origin
 */
#define ARM_ORIGIN_Y_MM 30

/**
 * @brief Workspace array
 */
static uint8_t workspace[WORKSPACE_SQMM][WORKSPACE_SQMM] = {{0}};

/**
 * @brief Number of known obstacles
 */
static int num_obstacles;

/**
 * @brief List of obstacles in workspace
 */
static struct rectangle obstacles[MAX_NUM_OBJ];

/**
 * @brief Configuration space array
 *
 * ARM_RANGE x ARM_RANGE grid of possible configurations for arms
 */
static uint8_t cspace[ARM_RANGE / ARM_DEGREE_INC][ARM_RANGE / ARM_DEGREE_INC] = {{0}};

/**
 * @brief Print the cspace to the console
 */
static void print_cspace() __attribute__((unused));

/**
 * @brief Print the workspace to the console
 */
static void print_workspace() __attribute__((unused));

static void print_cspace()
{
	for (int i = 0; i < ARM_RANGE; i++) {
		for (int j = 0; j < ARM_RANGE; j++) {
			printf("%d", cspace[i][j]);
		}
		printf("\n");
	}
}

void print_workspace()
{
	int num_rows = sizeof(workspace) / sizeof(workspace[0]);
	int num_cols = sizeof(workspace[0]) / sizeof(workspace[0][0]);

	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_cols; j++) {
			printf("%d", workspace[i][j]);
		}
		printf("\n");
	}
}

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
 * @brief Cross product helper function
 */
int cross_product_helper(struct segment seg, double x, double y)
{
	return (seg.x2 - seg.x1) * (y - seg.y1) - (seg.y2 - seg.y1) * (x - seg.x1);
}

/**
 * @brief Helper function to calculate if points are within rectangle
 */
static int is_point_in_rectangle_helper(struct rectangle *rect, int x, int y)
{
	int d1 = cross_product_helper(rect->bottom, x, y);
	int d2 = cross_product_helper(rect->left, x, y);
	int d3 = cross_product_helper(rect->top, x, y);
	int d4 = cross_product_helper(rect->right, x, y);

	/* The point is inside if it's on the same side of all rectangle edges */
	return (d1 >= 0 && d2 >= 0 && d3 >= 0 && d4 >= 0) ||
	       (d1 <= 0 && d2 <= 0 && d3 <= 0 && d4 <= 0);
}

/**
 * @brief Marks the rectangle in the workspace as occupied
 *
 * @param[in] obstacle The obstacle to be marked
 */
static void mark_obstacle_in_workspace(struct rectangle *obstacle)
{
	/* Get dimensions of workspace */
	int num_rows = sizeof(workspace) / sizeof(workspace[0]);
	int num_cols = sizeof(workspace[0]) / sizeof(workspace[0][0]);

	/* Put a bounding box around the obstacle that is axis aligned */
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

	/* Iterate through bounding box and mark points inside the rectangle to workspace */
	printf("min_x: %d\n", min_x);
	printf("max_x: %d\n", max_x);
	printf("min_y: %d\n", min_y);
	printf("max_y: %d\n", max_y);
	for (int y = min_y; y <= max_y; y++) {
		for (int x = min_x; x <= max_x; x++) {
			if (is_point_in_rectangle_helper(obstacle, x, y)) {
				if (x >= 0 && x < num_cols && y >= 0 && y < num_rows) {
					workspace[y][x] = 1;
				}
			}
		}
	}
}

int add_obstacle(struct rectangle *obstacle)
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
				cspace[theta0][j] = 1;
			}
			continue;
		}

		/*
		 * Our model does not rotate the axis, instead assume theta1 = 0 is
		 * perpendicular to theta0 since each arm is in series with each other.
		 */
		for (int theta1 = theta0 - (ARM_RANGE / 2); theta1 < theta0 + (ARM_RANGE / 2);
		     theta1 += ARM_DEGREE_INC) {

			double x1_delta;
			double y1_delta;

			ret = get_segment_endpoint_trig(ARM_LEN_MM, (double)theta1, &x1_delta,
							&y1_delta);
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
				cspace[theta0][theta1] = 1;
				continue;
			}
		}
	}

	LOG_INF("Finished Generating Configuration Space");

	// print_cspace();
	print_workspace();

	return 0;
}
