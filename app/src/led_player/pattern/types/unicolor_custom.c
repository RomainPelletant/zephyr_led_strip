/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>

#include <stdlib.h>
#include <math.h>

#include <led_player/pattern/types/unicolor_custom.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(unicolor_custom, CONFIG_APP_LOG_LEVEL);

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

/**< @brief Network L2 callback >*/

static uint16_t m_current_color = 0U;

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/**
 * @brief Enable DHCP
 *
 * @param void
 * @return int 0 OK
 */
static void unicolor_custom_process(struct led_rgb *pixel_array, uint8_t brightness, size_t led_numbers, uint32_t color);

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static uint32_t hsv_to_rgb32(uint16_t h)
{
    // Convert h (hsv: 0-65535) into degrees (0-360), multiply by 100 for precision
    uint32_t hf = (h * 36000U) / 65535U;

    // Saturation multiplied by 100 for precision
    uint32_t s = 100U;
    // Fixed value multiplied by 100 for precision
    uint32_t v = 100U;

    // Maximal intensity
    uint32_t c = (v * s) / 100U;

    // Intermdiate value
    uint32_t mod_hf = (hf % 6000U);
    uint32_t x = (c * (100U - abs((mod_hf * 2U / 100U) - 100U))) / 100U;

    // Minimal value
    uint32_t m = v - c;

    // RGB in percent
    uint32_t rf = 0U;
    uint32_t gf = 0U;
    uint32_t bf = 0U;

    if (hf < 6000U) {
        rf = c; gf = x; bf = 0U;
    } else if (hf < 12000U) {
        rf = x; gf = c; bf = 0U;
    } else if (hf < 18000U) {
        rf = 0U; gf = c; bf = x;
    } else if (hf < 24000U) {
        rf = 0U; gf = x; bf = c;
    } else if (hf < 30000U) {
        rf = x; gf = 0U; bf = c;
    } else {
        rf = c; gf = 0U; bf = x;
    }

    // Conversion des valeurs RVB en entiers 8 bits (0-255)
    uint8_t r = (rf + m) * 255U / 100U;
    uint8_t g = (gf + m) * 255U / 100U;
    uint8_t b = (bf + m) * 255U / 100U;


    // Combiner R, G, et B dans un uint32_t (0x00RRGGBB)
    return (r << 16) | (g << 8) | b;
}

static int unicolor_custom_set_color(uint32_t *color, uint32_t *selected_color)
{
    if (!color) {
        return -EFAULT;
    }

    if (m_current_color >= UINT16_MAX) {
        m_current_color = 0U;
    }

    *selected_color = m_current_color;
    *color = hsv_to_rgb32(m_current_color);
    LOG_INF("selected color %d, index: %d", *color, m_current_color);

    return 0;
}

static int unicolor_custom_get_color(uint32_t *color, uint32_t *selected_color)
{
    if (!color) {
        return -EFAULT;
    }

    *selected_color = m_current_color;
    *color = hsv_to_rgb32(m_current_color);

    return 0;
}

static void unicolor_custom_increment_color(void)
{
    m_current_color += 750U;
}

static void unicolor_custom_process(struct led_rgb *pixel_array, uint8_t brightness, size_t led_numbers, uint32_t color)
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

int pattern_unicolor_custom_init(struct pattern_interface *generic_pattern)
{
	if (!generic_pattern) {
		LOG_ERR("generic_iface or input_data NULL");
		return -EFAULT;
	}

	generic_pattern->pattern_process = &unicolor_custom_process;
	generic_pattern->set_color = &unicolor_custom_set_color;
    generic_pattern->get_color = &unicolor_custom_get_color;
    generic_pattern->increment_color = &unicolor_custom_increment_color;

    m_current_color = generic_pattern->selected_color;

	return 0;
}
