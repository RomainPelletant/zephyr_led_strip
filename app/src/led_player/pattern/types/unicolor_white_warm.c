/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>

#include <led_player/pattern/types/unicolor_white_warm.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(unicolor_white_warm, CONFIG_APP_LOG_LEVEL);

typedef struct {
    char name[30];  // Nom de la couleur
    uint32_t hex; // Code hexadécimal
} fixed_colors;

fixed_colors white_warm_colors[] = {
    {"White gold", 0xFF5A0A},
    {"Pale beige", 0xcd5a0a},
    {"Gold", 0xff4103},
    {"Cream", 0xff2d03},
    {"Champagne", 0xcd2d03},
    {"Yellow", 0xff4100},
    {"Laiton", 0xff2300},
    {"Orange", 0xff0f00}
};

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

const static uint8_t num_colors = sizeof(white_warm_colors) / sizeof(white_warm_colors[0]);

static uint8_t m_current_color = 0U;
/**< @brief Network L2 callback >*/

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/**
 * @brief Enable DHCP
 *
 * @param void
 * @return int 0 OK
 */
static void unicolor_white_warm_process(struct led_rgb *pixel_array, uint8_t brightness, size_t led_numbers, uint32_t color);

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static int set_color(uint32_t *color, uint32_t *selected_color)
{
    if (!color /*|| !selector*/) {
        return -EFAULT;
    }

    if (m_current_color >= num_colors) {
        m_current_color = 0U;
    }
    LOG_INF("selected color %s, index: %d", white_warm_colors[m_current_color].name, m_current_color);
    *selected_color = m_current_color;
    *color = white_warm_colors[m_current_color].hex;

    return 0;
}

static int get_color(uint32_t *color, uint32_t *selected_color)
{
    if (!color) {
        return -EFAULT;
    }

    *selected_color = m_current_color;
    *color = white_warm_colors[m_current_color].hex;

    return 0;
}

static void increment_color(void)
{
    ++m_current_color;
}

static void unicolor_white_warm_process(struct led_rgb *pixel_array, uint8_t brightness, size_t led_numbers, uint32_t color)
{
	uint8_t target_r = (color >> 16) & 0xFF;
	uint8_t target_g = (color >> 8) & 0xFF;
	uint8_t target_b = color & 0xFF;

	for (int i = 0; i < led_numbers; i++) {
		pixel_array[i].r = target_r/brightness;
		pixel_array[i].g = target_g/brightness;
		pixel_array[i].b = target_b/brightness;
	}

}

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

int pattern_unicolor_white_warm_init(struct pattern_interface *generic_pattern)
{
	if (!generic_pattern) {
		LOG_ERR("generic_iface or input_data NULL");
		return -EFAULT;
	}

	generic_pattern->pattern_process = &unicolor_white_warm_process;
	generic_pattern->set_color = &set_color;
    generic_pattern->get_color = &get_color;
    generic_pattern->increment_color = &increment_color;

    m_current_color = generic_pattern->selected_color;

	return 0;
}
