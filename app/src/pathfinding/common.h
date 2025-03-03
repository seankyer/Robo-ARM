/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_PATH_COMMON_H_
#define APP_PATH_COMMON_H_

/**
 * @brief Allowable tolerance for pathfinding
 *
 * It's unlikely that a combination of angles will exactly equal
 * the target X,Y coordinate, so we must define some allowable
 * error tolerance in mm
 */
#define ALLOWABLE_TOLERANCE_MM 1

/**
 * @brief Required mm of clearance between arm edge and obstacles
 */
#define REQUIRED_CLEARANCE_MM 3

/*===========================================
 *  Workspace and robot dimension definitions
 * ==========================================*/

/**
 * @brief Real-world 'workspace' in mm^2
 */
#define WORKSPACE_SQMM 395

/**
 * @brief Dimension of workspace 2D array
 */
#define WORKSPACE_DIMENSION WORKSPACE_SQMM

/**
 * @brief Arm length in mm (both arms same size)
 */
#define ARM_LEN_MM     100

/**
 * @brief Arm width in mm (both arms same size)
 */
#define ARM_WIDTH_MM   30  /* arm length in mm */

/**
 * @brief Arm range of motion in degrees
 */
#define ARM_RANGE      180

/**
 * @brief Arm degree steps
 */
#define ARM_DEGREE_INC 1

/**
 * @brief Arm X origin
 */
#define ARM_ORIGIN_X_MM WORKSPACE_SQMM / 2

/**
 * @brief Arm Y origin
 */
#define ARM_ORIGIN_Y_MM 30

/*==================================
 *  Configuration space definitions
 * =================================*/

/**
 * @brief Configuration space dimensions in degrees^2
 */
#define CSPACE_DIMENSION ARM_RANGE / ARM_DEGREE_INC

/**
 * @brief markers used in cspace and wspace maps
 */
enum markers {
	FREE = 0U,
	OCCUPIED = 1U,
        START_POINT = 2U,
        END_POINT = 3U,
        PATH = 4U,
};

#endif /* APP_PATH_COMMON_H_ */