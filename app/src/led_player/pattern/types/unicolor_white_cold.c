/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>

#include <led_player/pattern/types/unicolor_white_cold.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(unicolor_white_cold, CONFIG_APP_LOG_LEVEL);

typedef struct {
    char name[30];  // Nom de la couleur
    uint32_t hex; // Code hexadécimal
} fixed_colors;

fixed_colors white_cold_colors[] = {
    {"Cold white", 0xF5F9FF},
    {"Light blue", 0xEAF3FF},
    {"Lithtning silver", 0xE0E5EE},
    {"Artic white", 0xEDF8FF},
    {"Glacier grey", 0xD8DEE9},
    {"Alabaster blue", 0xD0E3F0}
};

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

const static uint8_t num_colors = sizeof(white_cold_colors) / sizeof(white_cold_colors[0]);
/**< @brief Network L2 callback >*/
static uint8_t m_current_color = 0U;

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/**
 * @brief Enable DHCP
 *
 * @param void
 * @return int 0 OK
 */
static void unicolor_white_cold_process(struct led_rgb *pixel_array, uint8_t brightness, size_t led_numbers, uint32_t color);

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static int unicolor_white_cold_set_color(uint32_t *color, uint32_t *selected_color)
{
    if (!color) {
        return -EFAULT;
    }

    if (m_current_color >= num_colors) {
        m_current_color = 0U;
    }
    LOG_INF("selected color %s, index: %d", white_cold_colors[m_current_color].name, m_current_color);
    *selected_color = m_current_color;
    *color = white_cold_colors[m_current_color].hex;

    return 0;
}

static int unicolor_white_cold_get_color(uint32_t *color, uint32_t *selected_color)
{
    if (!color) {
        return -EFAULT;
    }

    *selected_color = m_current_color;
    *color = white_cold_colors[m_current_color].hex;

    return 0;
}

static void unicolor_white_cold_increment_color(void)
{
    ++m_current_color;
}

static void unicolor_white_cold_process(struct led_rgb *pixel_array, uint8_t brightness, size_t led_numbers, uint32_t color)
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

int pattern_unicolor_white_cold_init(struct pattern_interface *generic_pattern)
{
	if (!generic_pattern) {
		LOG_ERR("generic_iface or input_data NULL");
		return -EFAULT;
	}

	generic_pattern->pattern_process = &unicolor_white_cold_process;
	generic_pattern->set_color = &unicolor_white_cold_set_color;
    generic_pattern->get_color = &unicolor_white_cold_get_color;
    generic_pattern->increment_color = &unicolor_white_cold_increment_color;

    m_current_color = generic_pattern->selected_color;

	return 0;
}
