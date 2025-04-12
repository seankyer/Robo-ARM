/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_UTILS_H_
#define APP_UTILS_H_

#include <lib/pathfind/spaces.h>
#include <lib/map_utils.h>

/**
 * @brief Marker in .txt file for CSPACE
 */
#define CSPACE_MARKER "===CSPACE==="

/**
 * @brief Marker in .txt file for WSPACE
 */
#define WSPACE_MARKER "===WSPACE==="

/**
 * @brief Marker in .txt file marking done
 */
#define SPACE_MARKER "===DONE==="

/**
 * @brief Print wspace and cspace to console with delimiters
 */
void print_spaces(uint8_t (*cspace)[CSPACE_DIMENSION], uint8_t (*wspace)[WORKSPACE_DIMENSION])
	__attribute__((unused));

#endif /* APP_UTILS_H_ */
