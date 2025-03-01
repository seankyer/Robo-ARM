/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_OBSTACLES_H_
#define APP_OBSTACLES_H_

#include <lib/map_utils.h>

struct rectangle obstacle1 = {
        .bottom = {
                .x1 = 40,
                .y1 = 40,
                .x2 = 44,
                .y2 = 40
        },
        .top = {
                .x1 = 40,
                .y1 = 44,
                .x2 = 44,
                .y2 = 44
        },
        .left = {
                .x1 = 40,
                .y1 = 40,
                .x2 = 40,
                .y2 = 44
        },
        .right = {
                .x1 = 44,
                .y1 = 40,
                .x2 = 44,
                .y2 = 44
        }
};

#endif /* APP_PATHFINDING_H_ */
