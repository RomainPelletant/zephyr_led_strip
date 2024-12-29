/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/sys/poweroff.h>
#include <esp_sleep.h>

#include <led_player/led_player.h>
#if defined(CONFIG_PM)
#include <low_power/low_power.h>
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(touch_controller, CONFIG_APP_LOG_LEVEL);

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

static const struct device *const touch_dev = DEVICE_DT_GET(DT_ALIAS(touch));
static const struct device *const long_touch_dev = DEVICE_DT_GET(DT_ALIAS(long_touch));

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

static void input_event_callback(struct input_event *evt, void *user_data);

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static void input_event_callback(struct input_event *evt, void *user_data)
{
	LOG_INF("event : %d", evt->code);
	if (evt->code == INPUT_KEY_0 && evt->value == 1U) {
		LOG_INF("INPUT_KEY_0");
		led_player_increment_mode();
	}
	if (evt->code == INPUT_KEY_1 && evt->value == 1U) {
		LOG_INF("INPUT_KEY_1");
		led_player_increment_color(255);
	}
	if (evt->code == INPUT_KEY_2 && evt->value == 1U) {
		LOG_INF("INPUT_KEY_2");
		led_player_increment_brightness(10);
	}
	if (evt->code == 21 && evt->value == 1U) {
		LOG_WRN("INPUT_KEY_Y: Power-off required!");
		// led_player_increment_brightness(10);
	}
}
INPUT_CALLBACK_DEFINE(touch_dev, input_event_callback, NULL);

static void long_input_event_callback(struct input_event *evt, void *user_data)
{
	LOG_INF("long event : %d", evt->code);
	if (evt->code == INPUT_KEY_Y && evt->value == 1U) {
		LOG_WRN("INPUT_KEY_Y: Power-off required!");
#if defined(CONFIG_PM)
		low_power_sleep();
#endif
#if defined(CONFIG_POWEROFF)
		sys_poweroff();
#endif
		// led_player_increment_brightness(10);
	}
}
INPUT_CALLBACK_DEFINE(long_touch_dev, long_input_event_callback, NULL);


/////////////////////////////////////
// Functions definition
/////////////////////////////////////
