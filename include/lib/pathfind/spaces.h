/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SPACES_H_
#define APP_SPACES_H_

#include <stdbool.h>
#include <stdint.h>
#include <lib/common.h>
#include <lib/map_utils.h>

/**
 * @brief Add a known obstacle to the environment
 */
int add_obstacle(const struct rectangle *obstacle);

/**
 * @brief Generates the configuration space given a workspace map
 *
 * @retval 0 on success, non-zero otherwise
 */
int generate_configuration_space(void);

/**
 * @brief Copy wspace to pointer
 *
 * @param[out] wspace Pointer to 2D workspace array
 *
 * @retval 0 on success, non-zero otherwise
 */
uint8_t (*get_wspace(void))[WORKSPACE_DIMENSION];

/**
 * @brief Copy cspace to pointer
 *
 * @param[out] cspace Pointer to 2D cspace array
 *
 * @retval 0 on success, non-zero otherwise
 */
uint8_t (*get_cspace(void))[CSPACE_DIMENSION];

/**
 * @brief Cleanup cspace and reset free markers once finished
 *
 * Iterates over cspace and sets any non-occupied grid square back to free
 */
void cleanup_cspace(void);

#endif /* APP_SPACES_H_ */
