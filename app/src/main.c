/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <app_version.h>
#include <zephyr/shell/shell.h>

#include <threads/control.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/**
 * Current control job
 */
static control_job_t job;

static int arm_go(const struct shell *shell, size_t argc, char **argv)
{
	int ret;
	char *end;

	if (argc != 3) {
		shell_error(shell, "Usage: go <x> <y>");
		return -EINVAL;
	}

	int x_coord = strtol(argv[1], &end, 10);
	int y_coord = strtol(argv[2], &end, 10);

	if (x_coord < 0 || x_coord > CONFIG_PATHFIND_WORKSPACE_SQMM) {
		shell_error(shell, "X-coordinate must be within bounds of workspace");
	}

	if (y_coord < 0 || y_coord > CONFIG_PATHFIND_WORKSPACE_SQMM) {
		shell_error(shell, "Y-coordinate must be within bounds of workspace");
	}

	job.x_coord = x_coord;
	job.y_coord = y_coord;
	job.demo = false;

	ret = control_submit_job(job);
	if (ret) {
		shell_error(shell, "Failed to submit job to control!");
	}

	return 0;
}

static int demo_movement(const struct shell *shell, size_t argc, char **argv)
{
	int ret;

	shell_print(shell, "Submitting demo job to control");

	job.demo = true;

	ret = control_submit_job(job);
	if (ret) {
		shell_error(shell, "Error submitting demo task!");
	}

	return 0;
}

int main(void)
{
	LOG_INF("Starting Robo-ARM!");

	return 0;
}

SHELL_CMD_REGISTER(demo, NULL, "Plays example movement", demo_movement);
SHELL_CMD_REGISTER(go, NULL, "Sets arm to given coordinates", arm_go);
