/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_OBSTACLES_H_
#define APP_OBSTACLES_H_

#include <lib/map_utils.h>

struct rectangle obstacle1 = {.bottom = {.x1 = 60, .y1 = 90, .x2 = 74, .y2 = 90},
			      .top = {.x1 = 60, .y1 = 104, .x2 = 74, .y2 = 104},
			      .left = {.x1 = 60, .y1 = 90, .x2 = 60, .y2 = 104},
			      .right = {.x1 = 74, .y1 = 90, .x2 = 74, .y2 = 104}};

#endif /* APP_PATHFINDING_H_ */
