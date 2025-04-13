/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ARM_CTRL_H
#define ARM_CTRL_H

#include <zephyr/kernel.h>
#include <lib/pathfind/pathfinding.h>

/**
 * @brief Arm job
 */
typedef struct {
	struct pathfinding_steps steps[MAX_NUM_STEPS]; /**< Pointer to array of steps to follow */
	size_t num_steps;                              /**< Number of steps in array */
} arm_job_t;

/**
 * @brief Submit a path for arm controller to follow
 *
 * @param[in] job Struct holding details of the job
 *
 * @returns 0 on success, otherwise failure
 */
int arm_ctrl_submit_job(arm_job_t job, int *last_coord_0, int *last_coord_1);

#endif // ARM_CTRL_H
