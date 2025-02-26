#include <stdio.h>
#include <stdlib.h>
#include <pathfinding.h>
#include <lib/map_utils.h>

/* Configuration space representation */
#define ARM_LEN_MM 100 /* Arm length in cm */
#define ARM_WIDTH_MM 30 /* Arm length in cm */
#define ARM_RANGE 180 /* Arm range of motion in degrees */

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
 * @brief Struct definition of an obstacle present on workspace
 */
struct obstacle {
        int16_t x;
        int16_t y;
};

static struct obstacle obstacles[MAX_NUM_OBJ];

/**
 * @brief Checks if coordinates collide with environment
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
static bool check_collisions(int16_t x, int16_t y)
{
        /* Distance in mm */
        int32_t distance;

        for (int i = 0; i < MAX_NUM_OBJ; i++) {
                distance = get_distance_from_line(ARM_ORIGIN_X, ARM_ORIGIN_Y, x, y, obstacles[i].x, obstacles[i].y);
        }

        return false;
}

int generate_configuration_space()
{
        return 0;
}
