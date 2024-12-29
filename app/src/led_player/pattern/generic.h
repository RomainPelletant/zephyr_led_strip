/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GENERIC_H
#define GENERIC_H

#include <zephyr/drivers/led_strip.h>

/**< @brief Generic interface states >*/

/**< @brief Generic interface initialize function pointer >*/
typedef void (*pattern_process_t)(struct led_rgb *pixel_array, uint8_t brightness, size_t led_numbers, uint32_t color);

/**< @brief Generic interface stop function pointer >*/
typedef int (*color_backend_t)(uint32_t *color, uint32_t *selected_color);

typedef void (*color_increment_t)(void);

/**< @brief Generic interface structure >*/
typedef struct pattern_interface {
	pattern_process_t pattern_process;
	color_backend_t set_color;
	color_backend_t get_color;
	color_increment_t increment_color;
	uint32_t selected_color;
} pattern_interface_t;

/**
 * @brief Interface implements ethernet interface
 *
 * @param[inout] g_iface: pointer to generic interface structure
 * @return int 0 OK
 */
int pattern_is_rainbow(struct pattern_interface *g_iface);

/**
 * @brief Interface implements wifi interface
 *
 * @param[inout] g_iface: pointer to generic interface structure
 * @return int 0 OK
 */
int pattern_is_unicolor_white_cold(struct pattern_interface *g_iface);

/**
 * @brief Interface implements LTE-M/NB-IoT interface
 *
 * @param[inout] g_iface: pointer to generic interface structure
 * @return int 0 OK
 */
int pattern_is_unicolor_white_warm(struct pattern_interface *g_iface);

/**
 * @brief Interface implements LoRa interface
 *
 * @param[inout] g_iface: pointer to generic interface structure
 * @return int 0 OK
 */
int pattern_is_unicolor_custom(struct pattern_interface *g_iface);

/**
 * @brief Get current interface
 * @warning not thread safe
 *
 * @param void
 * @return struct pattern_interface pointer to generic interface structure
 */
struct pattern_interface *generic_get_current_interface(void);

#endif