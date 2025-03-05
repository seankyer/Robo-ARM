/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <utils.h>

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
