/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SPACES_H_
#define APP_SPACES_H_

#include <stdbool.h>
#include <stdint.h>
#include <common.h>
#include <lib/map_utils.h>

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
