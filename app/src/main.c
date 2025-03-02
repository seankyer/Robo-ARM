/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <app_version.h>
#include <utils.h>
#include <spaces.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	int ret;

	/*
	 * Pointers to generated spaces
	 */
	uint8_t (*cspace)[CSPACE_DIMENSION];
	uint8_t (*wspace)[WORKSPACE_DIMENSION];

	/*
	 * Add known obstacles to workspace
	 */
	for (int i = 0; i < sizeof(obstacles)/sizeof(struct rectangle); i++) {
		ret = add_obstacle(&obstacles[i]);
		if (ret) {
			LOG_ERR("Error adding obstacle!");
		}
	}

	/*
	 * Generate cspace
	 */
	ret = generate_configuration_space();
	if (ret) {
		LOG_ERR("Couldn't generate configuration space");
	}

	/*
	 * Get and print spaces for python script
	 */
	get_cspace(&cspace);
	get_wspace(&wspace);
	print_spaces(cspace, wspace);

	/*
	 * Given two points in space, calculate the path
	 */

	return 0;
}
