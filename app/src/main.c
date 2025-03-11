/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <app_version.h>
#include <pathfinding.h>
#include <spaces.h>
#include <utils.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/**
 * @brief Array of known workspace obstacles
 */
static const struct rectangle obstacles[] = {

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

static void print_work(void)
{
	uint8_t(*cspace)[CSPACE_DIMENSION] = get_cspace();
	uint8_t(*wspace)[WORKSPACE_DIMENSION] = get_wspace();

	print_spaces(cspace, wspace);
}

int main(void)
{
	int ret;

	/*
	 * Add known obstacles to workspace
	 */
	for (int i = 0; i < sizeof(obstacles) / sizeof(struct rectangle); i++) {
		ret = add_obstacle(&obstacles[i]);
		if (ret) {
			LOG_ERR("Error adding obstacle! (err: %d)", ret);
		}
	}

	/*
	 * Generate cspace
	 */
	ret = generate_configuration_space();
	if (ret) {
		LOG_ERR("Couldn't generate configuration space");
		return ret;
	}

	struct pathfinding_steps plan[MAX_NUM_STEPS];
	int num_steps;

	/*
	 * Given two points in space, calculate the path.
	 */
	// ret = pathfinding_calculate_path(37, 1, 170, 200, plan, &num_steps);
	ret = pathfinding_calculate_path(4, 69, 170, 200, plan, &num_steps);
	// ret = pathfinding_calculate_path(53, 95, 170, 200, plan, &num_steps);
	// ret = pathfinding_calculate_path(75, 75, 170, 200, plan, &num_steps);
	// ret = pathfinding_calculate_path(40, 170, 170, 200, plan, &num_steps);
	// ret = pathfinding_calculate_path(162, 118, 170, 200, plan, &num_steps); /** Should fail
	// */
	if (ret) {
		LOG_ERR("Error during pathfinding (err: %d)", ret);
	}

	/* TODO: Submit plan to motor control thread */

	/* Print spaces for Python modelling */
	print_work();

	return 0;
}
