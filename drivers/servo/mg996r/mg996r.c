/*
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT mg996r_servo

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app/drivers/mg996r.h>

LOG_MODULE_REGISTER(mg996r_servo, LOG_LEVEL_INF);

struct mg996r_servo_config {
	const struct pwm_dt_spec pwm;
	uint32_t min_pulse_us;
	uint32_t max_pulse_us;
	bool inverted;
};

int mg996r_set_angle(const struct device *dev, uint8_t angle_deg)
{
	const struct mg996r_servo_config *cfg = dev->config;

	if (!device_is_ready(cfg->pwm.dev)) {
		LOG_ERR("PWM device not ready");
		return -ENODEV;
	}

	if (angle_deg > MG996R_MAX_ANGLE) {
		LOG_ERR("Angle out of range: %d", angle_deg);
		return -EINVAL;
	}

	/* If motor oriented in inverted, flip output */
	if (cfg->inverted) {
		angle_deg = MG996R_MAX_ANGLE - angle_deg;
	}

	uint32_t pulse_width =
		cfg->min_pulse_us +
		((cfg->max_pulse_us - cfg->min_pulse_us) * angle_deg) / MG996R_MAX_ANGLE;

	LOG_DBG("Setting pulse_width: %d", pulse_width);

	return pwm_set_pulse_dt(&cfg->pwm, pulse_width);
}

#define MG996R_DEFINE(inst)                                                                        \
	static const struct mg996r_servo_config mg996r_servo_cfg_##inst = {                        \
		.pwm = PWM_DT_SPEC_INST_GET(inst),                                                 \
		.min_pulse_us = DT_INST_PROP(inst, min_pulse),                                     \
		.max_pulse_us = DT_INST_PROP(inst, max_pulse),                                     \
		.inverted = DT_INST_PROP(inst, inverted),                                          \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(inst, NULL, NULL, NULL, &mg996r_servo_cfg_##inst, POST_KERNEL,       \
			      CONFIG_KERNEL_INIT_PRIORITY_DEVICE, NULL);

DT_INST_FOREACH_STATUS_OKAY(MG996R_DEFINE)
