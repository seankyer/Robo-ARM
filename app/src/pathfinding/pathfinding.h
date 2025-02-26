/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_PATHFINDING_H_
#define APP_PATHFINDING_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_NUM_OBJ 10

/**
 * @brief Generates the configuration space given a workspace map
 *
 * @retval 0 on success, non-zero otherwise
 */
int generate_configuration_space(void);

#endif /* APP_PATHFINDING_H_ */
