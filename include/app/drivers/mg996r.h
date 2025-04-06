/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_DRIVERS_MG996R_H_
#define APP_DRIVERS_MG996R_H_

#include <zephyr/device.h>
#include <zephyr/toolchain.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG996R_MIN_ANGLE 0
#define MG996R_MAX_ANGLE 180

int mg996r_set_angle(const struct device *dev, uint8_t angle_deg);

#ifdef __cplusplus
}
#endif

#endif /* APP_DRIVERS_MG996R_H_ */
