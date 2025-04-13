/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <arm_ctrl.h>
#include <app/drivers/mg996r.h>

LOG_MODULE_REGISTER(arm_control, LOG_LEVEL_INF);

static const struct device *servo0 = DEVICE_DT_GET(DT_NODELABEL(servo0));
static const struct device *servo1 = DEVICE_DT_GET(DT_NODELABEL(servo1));

#define ARM_STACK_SIZE 4096
#define ARM_PRIORITY   5

/**
 * @brief Message queue to hold one job at a time
 */
K_MSGQ_DEFINE(arm_job_queue, sizeof(arm_job_t), 1, 4);

/**
 * @brief Initialize the ARM
 */
static void initialize_arm(void)
{
	LOG_INF("Initializing arm!");

	if (!device_is_ready(servo0)) {
		LOG_ERR("servo0 device is not ready");
		return;
	}

	if (!device_is_ready(servo1)) {
		LOG_ERR("servo0 device is not ready");
		return;
	}

	if (mg996r_set_angle(servo0, 0)) {
		LOG_ERR("Error resetting servo0 angle!");
	}


	if (mg996r_set_angle(servo1, 0)) {
		LOG_ERR("Error resetting servo1 angle!");
	}

	/* Ensure movement completes - let's not get hasty */
	k_sleep(K_SECONDS(1));

	LOG_INF("Initialization complete!");
}

/**
 * @brief Job thread
 */
static void arm_job_thread_fn(void *p1, void *p2, void *p3)
{
	initialize_arm();

	int ret;
	arm_job_t job;

	while (1) {
		if (k_msgq_get(&arm_job_queue, &job, K_FOREVER) == 0) {
			LOG_INF("Received job with %d steps", (int)job.num_steps);

			for (size_t i = 0; i < job.num_steps; ++i) {
				struct pathfinding_steps step = job.steps[i];
				LOG_INF("Step %d -> theta0: %d, theta1: %d", (int)i, step.theta0, step.theta1);

				ret = mg996r_set_angle(servo0, step.theta0);
				if (ret) {
					LOG_ERR("Error setting servo0 angle!");
				}

				ret = mg996r_set_angle(servo1, step.theta1);
				if (ret) {
					LOG_ERR("Error setting servo1 angle!");
				}

				/* We have no feedback, so have to give time for servo to move */
				k_msleep(25);
			}

			// If steps were dynamically allocated, free here (optional)
			// k_free(job.steps);
		}
	}
}

K_THREAD_DEFINE(arm_job_thread_id, ARM_STACK_SIZE, arm_job_thread_fn, NULL, NULL, NULL,
		ARM_PRIORITY, 0, 0);

int arm_ctrl_submit_job(arm_job_t job, int *last_coord_0, int *last_coord_1)
{
	int ret;

	if (job.num_steps > MAX_NUM_STEPS) {
		LOG_ERR("Job exceeds max number of steps! Aborting!");
		return -EINVAL;
	}

	/* Submitting job is blocking */
	ret = k_msgq_put(&arm_job_queue, &job, K_FOREVER);
	if (ret) {
		return ret;
	}

	/* Update the current coordinates for main thread */
	*last_coord_0 = job.steps[job.num_steps - 1].theta0;
	*last_coord_1 = job.steps[job.num_steps - 1].theta1;

	return 0;
}

