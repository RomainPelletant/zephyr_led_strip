/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**< @brief Generic interface states >*/
enum led_player_mode {
	LED_PLAYER_MODE_UNICOLOR_WHITE,
	LED_PLAYER_MODE_UNICOLOR_WARM,
	LED_PLAYER_MODE_UNICOLOR_CUSTOM,
	LED_PLAYER_MODE_RAINBOW,
	LED_PLAYER_MODE_UNISHINE,
	LED_PLAYER_MODE_MAX
};

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

int led_player_init(uint32_t *led_length);

void led_player_set_mode(const enum led_player_mode mode);

enum led_player_mode led_player_get_mode(void);

void led_player_increment_mode(void);

void led_player_set_color(const uint32_t color);

uint32_t led_player_get_color(void);

void led_player_increment_color(uint32_t step);

void led_player_set_speed(const uint8_t speed);

uint8_t led_player_get_speed(void);

void led_player_set_brightness(uint8_t brightness);

uint8_t led_player_get_brightness(void);

void led_player_increment_brightness(uint8_t step);
