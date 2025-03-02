/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SPACES_H_
#define APP_SPACES_H_

#include <stdbool.h>
#include <stdint.h>
#include <lib/map_utils.h>

#define MAX_NUM_OBJ 10

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

#endif /* APP_SPACES_H_ */
