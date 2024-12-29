/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>

#include <led_player/pattern/generic.h>
#include <led_player/pattern/types/rainbow.h>
#include <zephyr/drivers/led_strip.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(rainbow, CONFIG_APP_LOG_LEVEL);

#define COLOR_MAX	255U

typedef struct {
    char name[30];  // Nom de la couleur
    uint32_t hex; // Code hexadécimal
} fixed_colors;

fixed_colors rainbow_colors[] = {
    {"RGB enabled", 0xFFFFFF},
    {"Red disabled", 0x00FFFF},
    {"Green disabled", 0xFF00FF},
    {"Blue disabled", 0xFFFF00}
};

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

/**< @brief Network L2 callback >*/

const static uint8_t num_colors = sizeof(rainbow_colors) / sizeof(rainbow_colors[0]);

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
static void rainbow_process(struct led_rgb *pixel_array, uint8_t brightness, size_t led_numbers, uint32_t color);

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static int rainbow_set_color(uint32_t *color, uint32_t *selected_color)
{
    if (!color) {
        return -EFAULT;
    }

    if (m_current_color >= num_colors) {
        m_current_color = 0U;
    }
    LOG_INF("selected color %s, index: %d", rainbow_colors[m_current_color].name, m_current_color);
	*selected_color = m_current_color;
    *color = rainbow_colors[m_current_color].hex;

    return 0;
}

static int rainbow_get_color(uint32_t *color, uint32_t *selected_color)
{
    if (!color) {
        return -EFAULT;
    }

	*selected_color = m_current_color;
    *color = rainbow_colors[m_current_color].hex;

    return 0;
}

static void rainbow_increment_color(void)
{
    ++m_current_color;
}

static void rainbow_process(struct led_rgb *pixel_array, uint8_t brightness, size_t led_numbers, uint32_t color)
{
	static uint32_t j;
	uint16_t red = 0U;
	uint16_t green = 0U;
	uint16_t blue = 0U;
	const int segment_size = led_numbers / 3;
	int k = 0;
	int l = 0;
	int m = 0;

	/* Color argument is just used to disable a specific color */
	uint8_t target_r = (color >> 16) & 0xFF;
	uint8_t target_g = (color >> 8) & 0xFF;
	uint8_t target_b = color & 0xFF;

	for (int i = 0; i < led_numbers; i++) {
		if ( ( i >= 0 ) && ( i < led_numbers * 1 / 3 ) ) {
			red = COLOR_MAX * k / segment_size;
			green = 0;
			blue = COLOR_MAX - (COLOR_MAX * k / segment_size );
			k++;
			}
		if ( ( i >= led_numbers * 1 / 3 ) && ( i < led_numbers * 2 / 3 ) ) {
			red = COLOR_MAX - (COLOR_MAX * l / segment_size );
			green = COLOR_MAX * l / segment_size;
			blue = 0;
			l++;
		}
		if ( ( i >= led_numbers * 2 / 3 ) && ( i < led_numbers * 3 / 3 ) ) {
			red = 0;
			green = COLOR_MAX - (COLOR_MAX * m / segment_size );
			blue = COLOR_MAX * m / segment_size;
			m++;
		}
		uint32_t index = (i + j) % led_numbers;

		/* Apply rainbow color palette */
		red = (target_r == 0) ? 0 : red;
		green = (target_g == 0) ? 0 : green;
		blue = (target_b == 0) ? 0 : blue;

		pixel_array[index].r = red / brightness;
		pixel_array[index].g = green / brightness;
		pixel_array[index].b = blue / brightness;

	}
	j = (j + 1) % led_numbers;
		
}

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

int pattern_rainbow_init(struct pattern_interface *generic_pattern)
{
	if (!generic_pattern) {
		LOG_ERR("generic_iface or input_data NULL");
		return -EFAULT;
	}

	generic_pattern->pattern_process = &rainbow_process;
	generic_pattern->set_color = rainbow_set_color;
    generic_pattern->get_color = rainbow_get_color;
    generic_pattern->increment_color = rainbow_increment_color;

	m_current_color = generic_pattern->selected_color;

	return 0;
}
