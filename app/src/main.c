/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <app_version.h>
#include <pathfinding.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
        int ret;

        ret = generate_configuration_space();
        if (ret) {
                LOG_ERR("Couldn't generate configuration space\n");
        }

        return 0;
}

