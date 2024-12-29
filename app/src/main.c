/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include <led_player/led_player.h>
#include <factory_settings/factory_settings.h>

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

static struct factory_data *factory_settings;

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

int main(void)
{
	if (factory_settings_read()) {
		LOG_ERR("Failed to read factory data: default configuration used");
	}

	factory_settings = factory_settings_get();

	led_player_init((uint32_t*) factory_settings->led_length);

	while(1) {
		k_sleep(K_SECONDS(1));
	}
	return 0;
}
