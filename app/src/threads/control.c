/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <lib/pathfind/pathfinding.h>
#include <lib/pathfind/spaces.h>
#include <threads/arm_ctrl.h>
#include <examples.h>
#include <control.h>

LOG_MODULE_REGISTER(control, LOG_LEVEL_INF);

#define CONTROL_STACK_SIZE 28000
#define CONTROL_PRIORITY   2

/**
 * Track servo positions
 */
static int servo0_d;
static int servo1_d;

/**
 * Static job container
 */
static arm_job_t arm_job;

/**
 * @brief Message queue to hold one job at a time
 */
K_MSGQ_DEFINE(control_queue, sizeof(control_job_t), 1, 4);

/**
 * @brief Initialize the ARM
 */
static int initialize(void)
{
	int ret;

	/* Initialize starting coordinates */
	servo0_d = 0;
	servo1_d = 0;

	arm_job.num_steps = 1;
	arm_job.steps[0].theta0 = 1;
	arm_job.steps[0].theta1 = 90;

	LOG_INF("Homing Robo-ARM to starting coordinates");

	ret = arm_ctrl_submit_job(arm_job, &servo0_d, &servo1_d);
	if (ret) {
		LOG_ERR("Error submitting example job to arm controller");
	}

	LOG_INF("Adding known obstacles to workspace");

	/*
	 * Add known obstacles to workspace
	 */
	ret = add_obstacle(&obstacles[2]);
	if (ret) {
		LOG_ERR("Error adding obstacle! (err: %d)", ret);
	}

	LOG_INF("Generating cspace!");

	/*
	 * Generate cspace
	 */
	ret = generate_configuration_space();
	if (ret) {
		LOG_ERR("Couldn't generate configuration space");
		return ret;
	}

	return 0;
}

static void demo_routine(void)
{
	int ret;

	LOG_INF("Control starting Robo-ARM demo");

	ret = arm_ctrl_submit_job(example_rotate_job, &servo0_d, &servo1_d);
	if (ret) {
		LOG_ERR("Error submitting example job to arm controller");
	}
}

/**
 * @brief Job thread
 */
static void control_thread_fn(void *p1, void *p2, void *p3)
{
	if (initialize()) {
		LOG_ERR("Critical failure! Control failed to initialize");
		return;
	}

	int ret;
	control_job_t job;

	LOG_INF("Control initialized, ready for command...");

	while (1) {
		if (k_msgq_get(&control_queue, &job, K_FOREVER) == 0) {

			if (job.demo) {
				demo_routine();
				continue;
			}

			LOG_INF("Calculating path to [%d, %d]", job.x_coord, job.y_coord);

			ret = pathfinding_calculate_path(servo0_d, servo1_d, job.x_coord, job.y_coord, arm_job.steps,
							 &arm_job.num_steps);
			if (ret) {
				LOG_ERR("Error during pathfinding (err: %d)", ret);
				cleanup_cspace();
				continue;
			}

			LOG_INF("Submitting job to arm control");

			ret = arm_ctrl_submit_job(arm_job, &servo0_d, &servo1_d);
			if (ret) {
				LOG_ERR("Error submitting arm task!");
				cleanup_cspace();
				continue;
			}

			/**
			 * Clean the cspace after calculation
			 */
			cleanup_cspace();

			LOG_INF("Control ready...");
		}
	}
}

K_THREAD_DEFINE(control_thread_id, CONTROL_STACK_SIZE, control_thread_fn, NULL, NULL, NULL,
        CONTROL_PRIORITY, 0, 0);

int control_submit_job(control_job_t job)
{
        return k_msgq_put(&control_queue, &job, K_FOREVER);
}
