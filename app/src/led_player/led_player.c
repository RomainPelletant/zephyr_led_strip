/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <stdlib.h>

#include <factory_settings/factory_settings.h>
#include <context_storage/context_storage.h>
#include <zephyr/drivers/led_strip.h>

#include <led_player/pattern/generic.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(led_player, CONFIG_APP_LOG_LEVEL);

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

/**< @brief Mutex protecting interface status access >*/
K_MUTEX_DEFINE(m_generic_mutex);

// /**< @brief Condvar protecting interface status access >*/
// K_CONDVAR_DEFINE(m_generic_state_cond);

/**< @brief Generic interface status to schedule FSM >*/
atomic_t m_mode = ATOMIC_INIT(0);
atomic_t m_color = ATOMIC_INIT(0);
atomic_t m_speed = ATOMIC_INIT(0);
atomic_t m_brightness = ATOMIC_INIT(100);

static struct context_data m_context_data;

#define STRIP_NODE		DT_ALIAS(led_strip)

// #if DT_NODE_HAS_PROP(DT_ALIAS(led_strip), chain_length)
// #define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)
// #else
// #error Unable to determine length of LED strip
// #endif
#define STRIP_NUM_PIXELS	14

#define DELAY_TIME K_MSEC(50)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

// static const struct led_rgb colors[] = {
// 	RGB(0x0f, 0x00, 0x00), /* red */
// 	RGB(0x00, 0x0f, 0x00), /* green */
// 	RGB(0x00, 0x00, 0x0f), /* blue */
// };

// static struct led_rgb pixels[STRIP_NUM_PIXELS];
struct led_rgb *pixel_array = NULL;

static size_t led_numbers;

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

#define ACQ_STACK_SIZE                1024
#define ACQ_THREAD_PRIORITY           8

k_tid_t thread_id;
struct k_thread thread_data;
K_THREAD_STACK_DEFINE(m_thread_stack, ACQ_STACK_SIZE);

static struct pattern_interface m_pattern_interface;

struct k_work_delayable work;

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static void save_context(struct k_work *item)
{
    // struct device_info *the_device =
    //     CONTAINER_OF(item, struct device_info, work);
    context_storage_write(&m_context_data);
	LOG_INF("SAVE CONTEXT");
}

static void led_player_loop(void *arg1, void *arg2, void *arg3)
{
	int err;
	uint32_t current_color = 0U;
	uint8_t scaled_brightness = 0U;

	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	while(1) {
		current_color = atomic_get(&m_color);
		scaled_brightness = 1 + ((100 - atomic_get(&m_brightness)) * 23 / 100);
		m_pattern_interface.pattern_process(pixel_array, scaled_brightness, led_numbers, current_color);

		err = led_strip_update_rgb(strip, pixel_array, led_numbers);
		if (err) {
			LOG_ERR("couldn't update strip: %d", err);
		}
		k_sleep(DELAY_TIME);
	}
}

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

int led_player_init(uint32_t *led_length)
{
	if (!led_length) {
		return -EFAULT;
	}

	if (device_is_ready(strip)) {
		LOG_INF("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return -ENODEV;
	}

	// 2m 40 / 1m 14 for 3m 54U / 145U
	led_numbers = (size_t) led_length;

	LOG_INF("Displaying pattern on strip");
	led_strip_set_length(strip, led_numbers);

	pixel_array = (struct led_rgb *) malloc(sizeof(struct led_rgb) * led_numbers);
	if (!pixel_array) {
		LOG_ERR("Failed to dynamically alloc LED array");
		return -EFAULT;
	}

	if (context_storage_read(&m_context_data)) {
		m_context_data.brightness = 70U;
		m_context_data.mode = 0;
		for(int i = 0; i < LED_PLAYER_MODE_MAX; i++) {
			m_context_data.pattern_context[i] = 0U;
		}
	}

	led_player_set_mode(m_context_data.mode);
	led_player_set_brightness(m_context_data.brightness);
	led_player_set_color(9U);

	k_work_init_delayable(&work, save_context);

	thread_id = k_thread_create(
		&thread_data, m_thread_stack,
		K_THREAD_STACK_SIZEOF(m_thread_stack), led_player_loop, NULL,
		NULL, NULL, ACQ_THREAD_PRIORITY, K_ESSENTIAL, K_NO_WAIT);

	return 0;
}

void led_player_set_mode(enum led_player_mode mode)
{
	enum led_player_mode previous_mode;
	k_mutex_lock(&m_generic_mutex, K_FOREVER);
	if (mode >= LED_PLAYER_MODE_UNISHINE) {
		mode = LED_PLAYER_MODE_UNICOLOR_WHITE;
	}
	previous_mode = atomic_set(&m_mode, mode);
	m_pattern_interface.selected_color = m_context_data.pattern_context[m_mode];
	switch (m_mode) {
		case LED_PLAYER_MODE_UNICOLOR_WHITE:
			pattern_is_unicolor_white_cold(&m_pattern_interface);
			led_player_set_color(32U);
		break;
		case LED_PLAYER_MODE_UNICOLOR_WARM:
			pattern_is_unicolor_white_warm(&m_pattern_interface);
			led_player_set_color(32U);
		break;

		case LED_PLAYER_MODE_UNICOLOR_CUSTOM:
			pattern_is_unicolor_custom(&m_pattern_interface);
			led_player_set_color(32U);
		break;

		case LED_PLAYER_MODE_RAINBOW:
			pattern_is_rainbow(&m_pattern_interface);
			led_player_set_color(32U);
		break;
		case LED_PLAYER_MODE_UNISHINE:

		break;
		default:

		break;
	}
	if (m_context_data.mode != m_mode) {
		m_context_data.mode = m_mode;
		k_work_reschedule(&work, K_SECONDS(10));
	}
	LOG_INF("led_player_set_mode %d previous was %d", mode, previous_mode);
	k_mutex_unlock(&m_generic_mutex);
}

enum led_player_mode led_player_get_mode(void)
{
	LOG_INF("led_player_get_mode");
	return atomic_get(&m_mode);
}

void led_player_increment_mode(void)
{
	k_mutex_lock(&m_generic_mutex, K_FOREVER);
	enum led_player_mode mode = led_player_get_mode();
	++mode;
	led_player_set_mode(mode);
	k_mutex_unlock(&m_generic_mutex);
}

void led_player_set_color(uint32_t color)
{
	if (color > 0xFFFFFF) {
		color = 0U;
	}
	k_mutex_lock(&m_generic_mutex, K_FOREVER);
	uint32_t co;
	uint32_t selected;
	m_pattern_interface.set_color(&co, &selected);
	// m_context_data.pattern_context[m_mode] = selected;
	uint32_t previous_mode = atomic_set(&m_color, co);
	LOG_INF("led_player_set_color %d previous was %d", color, previous_mode);

	if (m_context_data.pattern_context[m_mode] != selected) {
		m_context_data.pattern_context[m_mode] = selected;
		k_work_reschedule(&work, K_SECONDS(10));
	}
	// k_condvar_signal(&m_generic_state_cond);

	k_mutex_unlock(&m_generic_mutex);
}

uint32_t led_player_get_color(void)
{
	LOG_INF("led_player_get_mode");
	return atomic_get(&m_color);
}

void led_player_increment_color(uint32_t step)
{
	uint32_t color = led_player_get_color();
	m_pattern_interface.increment_color();
	// color = hsv_to_rgb32(hub);
	led_player_set_color(color);
	// LOG_INF("led_player_increment_color %d previous was %d", color, previous_color);
}

void led_player_set_speed(const uint8_t speed)
{
	k_mutex_lock(&m_generic_mutex, K_FOREVER);
	uint32_t previous_mode = atomic_set(&m_speed, speed);
	LOG_INF("led_player_set_speed %d previous was %d", speed, previous_mode);
	// k_condvar_signal(&m_generic_state_cond);
	k_mutex_unlock(&m_generic_mutex);
}

uint8_t led_player_get_speed(void)
{
	LOG_INF("led_player_get_speed");
	return atomic_get(&m_speed);
}

void led_player_set_brightness(uint8_t brightness)
{
	if (brightness > 100U) {
		brightness = 100U;
	}
	k_mutex_lock(&m_generic_mutex, K_FOREVER);
	uint32_t previous_mode = atomic_set(&m_brightness, brightness);
	LOG_INF("led_player_set_brightness %d previous was %d", brightness, previous_mode);
	// k_condvar_signal(&m_generic_state_cond);
	if (m_context_data.brightness != m_brightness) {
		m_context_data.brightness = m_brightness;
		k_work_reschedule(&work, K_SECONDS(10));
	}
	k_mutex_unlock(&m_generic_mutex);
}

// cppcheck-suppress unusedFunction
uint8_t led_player_get_brightness(void)
{
	LOG_INF("led_player_get_brightness");
	return atomic_get(&m_brightness);
}

void led_player_increment_brightness(uint8_t step)
{
	uint8_t brightness = led_player_get_brightness();
	if (brightness + step > 100U) {
		brightness = 0U;
	}
	// k_mutex_lock(&m_generic_mutex, K_FOREVER);
	LOG_WRN("%d", step);
	LOG_WRN("%d", brightness);
	brightness += step;
	LOG_WRN("%d", brightness);
	led_player_set_brightness(brightness);
	// k_mutex_unlock(&m_generic_mutex);
}

#include <zephyr/shell/shell.h>

static bool string_to_uint32(const char *str, uint32_t *res)
{
	char *end;
	errno = 0;
	intmax_t val = strtoimax(str, &end, 10);
	if (errno == ERANGE || val < 0 || val > UINT32_MAX || end == str || *end != '\0') {
		return false;
	}
	*res = (uint32_t)val;
	return true;
}

static int color(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t value;
	if (!argv[1] || strlen(argv[1]) > strlen("4294967295")) {
		shell_error(sh, "Time exceed %u characters", strlen("4294967295"));
		return -ENOSPC;
	}

	if (!string_to_uint32(argv[1], &value)) {
		shell_error(sh, "Error while converting to uint16_t");
		return -EINVAL;
	}
	// uint32_t color = hsv_to_rgb32(value);
	// led_player_set_color(color);
	return 0;
}

static int set_custom_color(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t valuer;
	uint32_t valueg;
	uint32_t valueb;
	if (!argv[1] || strlen(argv[1]) > strlen("4294967295")) {
		shell_error(sh, "Time exceed %u characters", strlen("4294967295"));
		return -ENOSPC;
	}

	if (!string_to_uint32(argv[1], &valuer)) {
		shell_error(sh, "Error while converting to uint16_t");
		return -EINVAL;
	}

	if (!string_to_uint32(argv[2], &valueg)) {
		shell_error(sh, "Error while converting to uint16_t");
		return -EINVAL;
	}

	if (!string_to_uint32(argv[3], &valueb)) {
		shell_error(sh, "Error while converting to uint16_t");
		return -EINVAL;
	}

	uint32_t color = (valuer << 16) | (valueg << 8) | valueb;
	LOG_INF("color %x", color);
	// uint32_t color = hsv_to_rgb32(value);
	// led_player_set_color(color);
	atomic_set(&m_color, color);
	return 0;
}

static int increment(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	led_player_increment_color(5);

	return 0;
}


/** @brief Lowpower shell categorie */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_app, SHELL_CMD_ARG(color, NULL, "set color",
					     set_custom_color, 4, 0),
						 SHELL_CMD(inc, NULL, "Get RGBW", increment),
			       SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(ledstrip, &sub_app, "LED-strip commands", NULL);
