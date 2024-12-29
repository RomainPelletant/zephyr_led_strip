/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>
#include <zephyr/drivers/gpio.h>
#include <esp_sleep.h>
#include <driver/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(low_power, CONFIG_APP_LOG_LEVEL);

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

static const struct gpio_dt_spec wakeup_pin =
	GPIO_DT_SPEC_GET(DT_ALIAS(wakeup_pin), gpios);

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////


/////////////////////////////////////
// Local functions definition
/////////////////////////////////////


/////////////////////////////////////
// Functions definition
/////////////////////////////////////

/** @brief Overload default policy state (to no policy)
 * @warning this function indicates to Zephyr that the policy
 * is handled manually
 */
const struct pm_state_info *pm_policy_next_state(uint8_t cpu, int32_t ticks)
{
	const struct pm_state_info *cpu_states;
	pm_state_cpu_get_all(cpu, &cpu_states);

	return NULL;
}


int low_power_sleep(void)
{
	const int wakeup_level = (wakeup_pin.dt_flags & GPIO_ACTIVE_LOW) ? 0 : 1;

	esp_gpio_wakeup_enable(wakeup_pin.pin,
			wakeup_level == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);

	esp_sleep_enable_gpio_wakeup();

	return 0;
}
