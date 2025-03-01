/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_OBSTACLES_H_
#define APP_OBSTACLES_H_

#include <lib/map_utils.h>

struct rectangle obstacles[] = {

	/* Rectangle off to the left of arm */
	{.bottom = {.x1 = 60, .y1 = 90, .x2 = 74, .y2 = 90},
	.top = {.x1 = 60, .y1 = 104, .x2 = 74, .y2 = 104},
	.left = {.x1 = 60, .y1 = 90, .x2 = 60, .y2 = 104},
	.right = {.x1 = 74, .y1 = 90, .x2 = 74, .y2 = 104}},

	/* Rectangle directly above arm and middle */
	{.bottom = {.x1 = 200, .y1 = 200, .x2 = 225, .y2 = 200},
	.top = {.x1 = 200, .y1 = 260, .x2 = 225, .y2 = 260},
	.left = {.x1 = 200, .y1 = 200, .x2 = 200, .y2 = 260},
	.right = {.x1 = 225, .y1 = 200, .x2 = 225, .y2 = 260}},

	/* Rectangle right very close to base */
	// {.bottom = {.x1 = 200, .y1 = 50, .x2 = 225, .y2 = 50},
	// .top = {.x1 = 200, .y1 = 70, .x2 = 225, .y2 = 70},
	// .left = {.x1 = 200, .y1 = 50, .x2 = 200, .y2 = 70},
	// .right = {.x1 = 225, .y1 = 50, .x2 = 225, .y2 = 70}},
};

#endif /* APP_PATHFINDING_H_ */
