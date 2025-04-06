/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <app/drivers/mg996r.h>

static const struct device *servo0 = DEVICE_DT_GET(DT_NODELABEL(servo0));
static const struct device *servo1 = DEVICE_DT_GET(DT_NODELABEL(servo1));

#define STEP 5 /* Step in degrees */

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

		k_sleep(K_MSEC(50));
	}

	return 0;
}
