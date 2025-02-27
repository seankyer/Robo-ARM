#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <pathfinding.h>
#include <lib/map_utils.h>

LOG_MODULE_REGISTER(pathfinding, LOG_LEVEL_INF);

#define REQUIRED_CLEARANCE_MM 5 /* mm of clearance between arm edge and obstacles */

/* Configuration space representation */
#define ARM_LEN_MM 100 /* arm length in mm */
#define ARM_WIDTH_MM 30 /* arm length in mm */
#define ARM_RANGE 180 /* arm range of motion in degrees */
#define ARM_DEGREE_INC 1 /* degrees increment */

/* Real-world 'workspace' representation */
#define WORKSPACE_STUDS 50
#define STUD_DIAMETER_MM 750

/**
 * @brief Arm origin
 *
 * What the Arm's origin point is
 */
#define ARM_ORIGIN_X WORKSPACE_STUDS / 2
#define ARM_ORIGIN_Y WORKSPACE_STUDS / 4

/**
 * @brief Workspace array
 *
 * 2D representation of each stud in our workspace
 */
static uint8_t workspace[WORKSPACE_STUDS][WORKSPACE_STUDS] = { {0} };

/**
 * @brief List of obstacles in workspace
 */
static struct rectangle obstacles[MAX_NUM_OBJ];

/**
 * @brief Configuration space array
 *
 * ARM_RANGE x ARM_RANGE grid of possible configurations for arms
 */
static uint8_t cspace[ARM_RANGE / ARM_DEGREE_INC][ARM_RANGE / ARM_DEGREE_INC] = { {0} };


/**
 * @brief Print the cspace to the console
 */
static void print_cspace()
{
        for (int i = 0; i < ARM_RANGE; i++) {
                for (int j = 0; j < ARM_RANGE; j++) {
                        printf("%d", cspace[i][j]);
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
        /* TODO: Need to loop the thickness of the arm to check for collisions */
        LOG_DBG("Checking collisions for segment spanning from (%f, %f) to (%f, %f)", orig_x, orig_y, end_x, end_y);

        struct segment seg = {
                .x1 = orig_x,
                .y1 = orig_y,
                .x2 = end_x,
                .y2 = end_y
        };

        /*
         * For each object
         */
        for (int i = 0; i < MAX_NUM_OBJ; i++) {

                /*
                 * Scan the thickness of our arm
                 */
                for (int i = -(ARM_WIDTH_MM / 2) - REQUIRED_CLEARANCE_MM; i <= (ARM_WIDTH_MM / 2) + REQUIRED_CLEARANCE_MM; i += (ARM_WIDTH_MM + (REQUIRED_CLEARANCE_MM * 2)) / 4) {

                        /* TODO: We need to translate the center coordinates of the arm along its parallel by the magnitude calculated by i (thickness of the arm + clearance requirement)*/
                }

                if (check_segment_rectangle_collisions(seg, obstacles[i])) {
                        return true;
                }
        }

        return false;
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

                double x0_endpoint = ARM_ORIGIN_X + x0_delta;
                double y0_endpoint = ARM_ORIGIN_Y + y0_delta;

                /*
                 * Calculate collisions in workspace
                 */
                if (check_collisions(ARM_ORIGIN_X, ARM_ORIGIN_Y, x0_endpoint, y0_endpoint)) {
                        for (int j = 0; j < ARM_RANGE; j += ARM_DEGREE_INC) {
                                LOG_INF("Recording collision at angles: (theta0: %d, theta1: %d)", theta0 ,j);
                                cspace[theta0][j] = 1;
                        }
                        continue;
                }

                /*
                 * Our model does not rotate the axis, instead assume theta1 = 0 is
                 * perpendicular to theta0 since each arm is in series with each other.
                 */
                for (int theta1 = theta0 - (ARM_RANGE / 2); theta1 < theta0 + (ARM_RANGE / 2); theta1 += ARM_DEGREE_INC) {

                        double x1_delta;
                        double y1_delta;

                        ret = get_segment_endpoint_trig(ARM_LEN_MM, (double)theta1, &x1_delta, &y1_delta);
                        if (ret) {
                                LOG_ERR("Error during segment endpoint calculation (err: %d)\n", ret);
                                return ret;
                        }

                        double x1_endpoint = x0_endpoint + x1_delta;
                        double y1_endpoint = y0_endpoint + y1_delta;


                        /*
                        * Calculate collisions in workspace
                        */
                        if (check_collisions(x0_endpoint, y0_endpoint, x1_endpoint, y1_endpoint)) {
                                LOG_INF("Recording collision at angles: (theta0: %d, theta1: %d)", theta0, theta1);
                                cspace[theta0][theta1] = 1;
                                continue;
                        }
                }
        }

        LOG_INF("Finished Generating Configuration Space");

        // print_cspace();

        return 0;
}
