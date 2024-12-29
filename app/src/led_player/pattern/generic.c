/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "types/rainbow.h"
#include "types/unicolor_custom.h"
#include "types/unicolor_white_cold.h"
#include "types/unicolor_white_warm.h"

#include "generic.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(generic, CONFIG_APP_LOG_LEVEL);

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

/**< @brief Generic interface container >*/
static struct pattern_interface *_m_pattern_iface = NULL;

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

int pattern_is_rainbow(struct pattern_interface *g_iface)
{
	LOG_WRN("Rainbow selected");
	_m_pattern_iface = g_iface;
	return pattern_rainbow_init(g_iface);
}

int pattern_is_unicolor_white_cold(struct pattern_interface *g_iface)
{
	LOG_WRN("Unicolor white cold selected");
	_m_pattern_iface = g_iface;
	return pattern_unicolor_white_cold_init(g_iface);
}

int pattern_is_unicolor_white_warm(struct pattern_interface *g_iface)
{
	LOG_WRN("Unicolor white warm selected");
	_m_pattern_iface = g_iface;
	return pattern_unicolor_white_warm_init(g_iface);
}

int pattern_is_unicolor_custom(struct pattern_interface *g_iface)
{
	LOG_WRN("Unicolor custom selected");
	_m_pattern_iface = g_iface;
	return pattern_unicolor_custom_init(g_iface);
}

// cppcheck-suppress unusedFunction
struct pattern_interface *generic_get_current_pattern(void)
{
	return _m_pattern_iface;
}
