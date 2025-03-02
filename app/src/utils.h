/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_OBSTACLES_H_
#define APP_OBSTACLES_H_

#include <stdio.h>
#include <spaces.h>
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
static void print_spaces() __attribute__((unused));

static void print_cspace(uint8_t (*cspace)[CSPACE_DIMENSION])
{
	/* Print cspace marker */
	printf("%s\n", CSPACE_MARKER);

	for (int i = 0; i < CSPACE_DIMENSION; i++) {
		for (int j = 0; j < CSPACE_DIMENSION; j++) {
			printf("%d", cspace[i][j]);
		}
		printf("\n");
	}

	/* Printf close cspace drawing */
	printf("%s\n", CSPACE_MARKER);
}

static void print_wspace(uint8_t (*wspace)[WORKSPACE_DIMENSION])
{
	/* Print wspace marker */
	printf("%s\n", WSPACE_MARKER);

	for (int i = 0; i < WORKSPACE_DIMENSION; i++) {
		for (int j = 0; j < WORKSPACE_DIMENSION; j++) {
			printf("%d", wspace[i][j]);
		}
		printf("\n");
	}

	/* Printf close wspace drawing */
	printf("%s\n", WSPACE_MARKER);
}

void print_spaces(uint8_t (*cspace)[CSPACE_DIMENSION], uint8_t (*wspace)[WORKSPACE_DIMENSION])
{
	print_cspace(cspace);
	print_wspace(wspace);

	/* Print done message */
	printf("%s\n", SPACE_MARKER);
}

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
