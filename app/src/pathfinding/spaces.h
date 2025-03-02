/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SPACES_H_
#define APP_SPACES_H_

#include <stdbool.h>
#include <stdint.h>
#include <lib/map_utils.h>

/**
 * @brief Upper limit for number of obstacles
 */
#define MAX_NUM_OBJ 10

/*
 * Workspace and robot dimension definitions
 */

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

/*
 * Configuration space definitions
 */

/**
 * @brief Configuration space dimensions in degrees^2
 */
#define CSPACE_DIMENSION ARM_RANGE / ARM_DEGREE_INC

/**
 * @brief Add a known obstacle to the environment
 */
int add_obstacle(struct rectangle *obstacle);

/**
 * @brief Generates the configuration space given a workspace map
 *
 * @retval 0 on success, non-zero otherwise
 */
int generate_configuration_space(void);

/**
 * @brief Get a pointer to the workspace
 *
 * @param[out] wspace Pointer to 2D workspace array
 *
 * @retval 0 on success, non-zero otherwise
 */
int get_wspace(uint8_t (**wspace_out)[WORKSPACE_DIMENSION]);

/**
 * @brief Get a pointer to the configuration space
 *
 * @param[out] cspace Pointer to 2D cspace array
 *
 * @retval 0 on success, non-zero otherwise
 */
int get_cspace(uint8_t (**cspace_out)[CSPACE_DIMENSION]);

#endif /* APP_SPACES_H_ */
