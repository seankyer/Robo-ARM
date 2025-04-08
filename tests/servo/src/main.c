/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/shell/shell.h>

#include <app/drivers/mg996r.h>

static const struct device *servo0 = DEVICE_DT_GET(DT_NODELABEL(servo0));
static const struct device *servo1 = DEVICE_DT_GET(DT_NODELABEL(servo1));

#define STEP 5 /* Step in degrees */

static bool rotate = false;

enum direction {
	DOWN,
	UP,
};

int main(void)
{
	int ret;
	int degrees = 0;
	enum direction dir = UP;

	printk("Servomotor control\n");

	if (!device_is_ready(servo0)) {
		printk("Error: servo0 device is not ready\n");
		return 0;
	}

	if (!device_is_ready(servo1)) {
		printk("Error: servo0 device is not ready\n");
		return 0;
	}

	while (1) {
		if (rotate) {
			printk("Setting degrees: %d\n", degrees);

			ret = mg996r_set_angle(servo0, degrees);
			if (ret) {
				printk("Error setting mg996r angle! (ret = %d)", ret);
			}

			ret = mg996r_set_angle(servo1, degrees);
			if (ret) {
				printk("Error setting mg996r angle! (ret = %d)", ret);
			}

			/* Rotate back and forth */
			if (dir == DOWN) {
				if (degrees <= MG996R_MIN_ANGLE) {
					dir = UP;
					degrees = MG996R_MIN_ANGLE;
				} else {
					degrees -= STEP;
				}
			} else {
				degrees += STEP;

				if (degrees >= MG996R_MAX_ANGLE) {
					dir = DOWN;
					degrees = MG996R_MAX_ANGLE;
				}
			}
		}

		k_sleep(K_MSEC(50));
	}

	return 0;
}

static int start_rotate(const struct shell *shell, size_t argc, char **argv)
{

	if (rotate) {
		rotate = false;
	} else {
		rotate = true;
	}

	shell_print(shell, "Rotate now: %d!", rotate);

	return 0;
}

static int servo_set(const struct shell *shell, size_t argc, char **argv)
{
	int ret;

	if (argc != 3) {
		shell_error(shell, "Usage: servo <servo_id> <degrees>");
		return -EINVAL;
	}

	char *end;
	int servo_id = strtol(argv[1], &end, 10);
	int degrees = strtol(argv[2], &end, 10);

	if (servo_id < 0 || servo_id > 2) {
		shell_error(shell, "Servo ID must be 0-2 inclusive");
		return -EINVAL;
	}

	if (servo_id != 1) {
		ret = mg996r_set_angle(servo0, degrees);
		if (ret) {
			printk("Error setting mg996r angle! (ret = %d)", ret);
			return ret;
		}
	}

	if (servo_id != 0) {
		ret = mg996r_set_angle(servo1, degrees);
		if (ret) {
			printk("Error setting mg996r angle! (ret = %d)", ret);
			return ret;
		}
	}

	shell_print(shell, "Set servos to %d\xC2\xB0", degrees);

	return 0;
}

SHELL_CMD_REGISTER(rotate, NULL, "Starts servo rotation", start_rotate);
SHELL_CMD_REGISTER(set, NULL, "Sets servo to a given degree", servo_set);

