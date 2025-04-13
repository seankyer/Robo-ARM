/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <app_version.h>

#include <lib/pathfind/pathfinding.h>
#include <lib/pathfind/spaces.h>
#include <threads/arm_ctrl.h>
#include <examples.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static int servo0_d;
static int servo1_d;

int main(void)
{
	int ret;

	LOG_INF("Starting Robo-ARM!");

	/* Initialize starting coordinates */
	servo0_d = 0;
	servo1_d = 0;

	LOG_INF("Submitting example rotate job");

	ret = arm_ctrl_submit_job(example_rotate_job, &servo0_d, &servo1_d);
	if (ret) {
		LOG_ERR("Error submitting arm task!");
	}

	k_yield();

	// /*
	//  * Add known obstacles to workspace
	//  */
	// for (int i = 0; i < sizeof(obstacles) / sizeof(struct rectangle); i++) {
	// 	ret = add_obstacle(&obstacles[i]);
	// 	if (ret) {
	// 		LOG_ERR("Error adding obstacle! (err: %d)", ret);
	// 	}
	// }

	LOG_INF("Generating cspace!");

	/*
	 * Generate cspace
	 */
	ret = generate_configuration_space();
	if (ret) {
		LOG_ERR("Couldn't generate configuration space");
		return ret;
	}

	LOG_INF("Calculating path to 170, 200");

	arm_job_t example_job;

	ret = pathfinding_calculate_path(servo0_d, servo1_d, 170, 170, example_job.steps, &example_job.num_steps);
	if (ret) {
		LOG_ERR("Error during pathfinding (err: %d)", ret);
	}

	LOG_INF("Running example job!");

	ret = arm_ctrl_submit_job(example_job, &servo0_d, &servo1_d);
	if (ret) {
		LOG_ERR("Error submitting arm task!");
	}

	return 0;
}
