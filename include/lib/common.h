/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_PATH_COMMON_H_
#define APP_PATH_COMMON_H_

/**
 * @brief Dimension of workspace 2D array
 */
#define WORKSPACE_DIMENSION CONFIG_PATHFIND_WORKSPACE_SQMM

/**
 * @brief Configuration space dimensions in degrees^2
 */
#define CSPACE_DIMENSION CONFIG_PATHFIND_ARM_RANGE / CONFIG_PATHFIND_ARM_DEGREE_INC

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