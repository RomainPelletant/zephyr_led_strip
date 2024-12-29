/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/sys/crc.h>

#include <factory_settings/factory_settings.h>

#if CONFIG_SHELL
#include <zephyr/shell/shell.h>
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(factory_settings, CONFIG_APP_LOG_LEVEL);

#define FACTORY_PARTITION factory_partition
#define FACTORY_DEVICE    FIXED_PARTITION_DEVICE(FACTORY_PARTITION)
#define FACTORY_AREA_ID   FIXED_PARTITION_ID(FACTORY_PARTITION)
#define FACTORY_SIZE      FIXED_PARTITION_SIZE(FACTORY_PARTITION)

#define FACTORY_FORMAT_REV    0x01

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

/**< @brief Current data container >*/
static struct factory_data m_factory_data = {
	.led_length = 14U,
	.rgbw = false,
};

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/**
 * @brief Check if magic word is correct
 *
 * @param[inout] data:  pointer of struct factory_data to be checked
 * @return bool true if OK / false if NOK
 */
static bool is_magic_number_valid(const struct factory_data *data);

/**
 * @brief Check CRC value of factory_data container structure
 *
 * @param[inout] data:  pointer of struct factory_data to be checked
 * @return bool true if OK / false if NOK
 */
static bool is_crc_valid(struct factory_data *data);

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static bool is_magic_number_valid(const struct factory_data *data)
{
	if (strncmp(data->magic, FACTORY_MAGIC_WORD, sizeof(FACTORY_MAGIC_WORD)) == 0) {
		return true;
	}

	return false;
}

static bool is_crc_valid(struct factory_data *data)
{
	uint32_t crc = crc32_ieee((void *)data, sizeof(struct factory_data) - sizeof(uint32_t));
	if (crc != data->crc32) {
		LOG_ERR("CRC NOK");
		return false;
	}

	return true;
}

static void display_current_data(void)
{
	LOG_INF(" ------ [Factory settings] ------");
	LOG_INF("| magic %s", m_factory_data.magic);
	LOG_INF("| led_number %d", m_factory_data.led_length);
	LOG_INF("| rgbw %d", m_factory_data.rgbw);
	LOG_INF("| format_revision %d", m_factory_data.format_revision);
	LOG_INF(" --------------------------------");
}


#if defined(CONFIG_SHELL)

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

static int get_led_number(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (factory_settings_read()) {
		shell_error(sh, "Failed to read memory");
	} else {
		shell_print(sh, "LED number: %u", m_factory_data.led_length);
	}

	return 0;
}

static int set_led_number(const struct shell *sh, size_t argc, char **argv)
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

	if (value > 10000U) {
		shell_error(sh, "Don't abuse budy: more than 10k LEDs");
	} else {
		m_factory_data.led_length = value;
		if (factory_settings_write(&m_factory_data)) {
			shell_error(sh, "Failed to write in memory");
		} else {
			shell_print(sh, "OK:%u", m_factory_data.led_length);
		}
	}

	return 0;
}

int get_rgbw(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (factory_settings_read()) {
		shell_error(sh, "Failed to read memory");
	} else {
		shell_print(sh, "RGBW option: %s", m_factory_data.rgbw ? "on" : "off");
	}

	return 0;
}

int disable_rgbw(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	m_factory_data.rgbw = false;
	if (factory_settings_write(&m_factory_data)) {
		shell_error(sh, "Failed to write in memory");
	} else {
		shell_print(sh, "OK:%d", m_factory_data.rgbw);
	}

	return 0;
}

int enable_rgbw(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	m_factory_data.rgbw = true;
	if (factory_settings_write(&m_factory_data)) {
		shell_error(sh, "Failed to write in memory");
	} else {
		shell_print(sh, "OK:%d", m_factory_data.rgbw);
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
	m_sub_led_number,
	SHELL_CMD(get, NULL, "Get LED number", get_led_number),
	SHELL_CMD_ARG(set, NULL, "Set LED number", set_led_number, 2, 0),
	SHELL_SUBCMD_SET_END);
SHELL_STATIC_SUBCMD_SET_CREATE(
	m_sub_rgbw,
	SHELL_CMD(get, NULL, "Get RGBW", get_rgbw),
	SHELL_CMD(off, NULL, "Disable RGBW", disable_rgbw),
	SHELL_CMD(on, NULL, "Enable TGBW", enable_rgbw),
	SHELL_SUBCMD_SET_END);


/** @brief Lowpower shell categorie */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_app,
						 SHELL_CMD_ARG(led_number, &m_sub_led_number, "Get/set LED number", NULL, 1, 1),
						 SHELL_CMD_ARG(rgbw, &m_sub_rgbw, "Enable/get RGBW", NULL, 1, 1),
			       SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(factory, &sub_app, "Factory settings", NULL);

#endif

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

int factory_settings_read(void)
{
	const struct flash_area *flash_area;

	if (flash_area_open(FACTORY_AREA_ID, &flash_area) != 0) {
		LOG_ERR("Failed to open eeprom");
		flash_area_close(flash_area);
		return -ENODEV;
	}

	if (flash_area_read(flash_area, 0, &m_factory_data, sizeof(struct factory_data)) != 0) {
		LOG_ERR("Failed to read EEPROM data config");
		flash_area_close(flash_area);
		return -EIO;
	}

	/* Check magic word for data comissioning check */
	if (!is_magic_number_valid(&m_factory_data)) {
		LOG_ERR("Wrong magic number");
		flash_area_close(flash_area);
		return -ENODATA;
	}

	display_current_data();
	/* Check CRC value for data integrity check */
	if (!is_crc_valid(&m_factory_data)) {
		LOG_ERR("Failed to check CRC data device");
		flash_area_close(flash_area);
		return -EBADMSG;
	}

	flash_area_close(flash_area);

	return 0;
}

int factory_settings_write(struct factory_data *data)
{
	const struct flash_area *flash_area;
	uint32_t crc;

	if (flash_area_open(FACTORY_AREA_ID, &flash_area) != 0) {
		LOG_ERR("Failed to open eeprom");
		flash_area_close(flash_area);
		return -ENODEV;
	}

	strncpy(data->magic, FACTORY_MAGIC_WORD, sizeof(FACTORY_MAGIC_WORD));

	/* Set current format revision */
	data->format_revision = FACTORY_FORMAT_REV;

	/* Compute CRC */
	crc = crc32_ieee((void *)data, sizeof(struct factory_data) - sizeof(uint32_t));
	data->crc32 = crc;

	if (flash_area_erase(flash_area, 0, FACTORY_SIZE) != 0) {
		LOG_ERR("Failed to write EEPROM data device");
		flash_area_close(flash_area);
		return -EIO;
	}

	if (flash_area_write(flash_area, 0, data, sizeof(struct factory_data)) != 0) {
		LOG_ERR("Failed to write EEPROM data device");
		flash_area_close(flash_area);
		return -EIO;
	}

	display_current_data();

	flash_area_close(flash_area);

	return 0;
}

struct factory_data *factory_settings_get(void)
{
	return &m_factory_data;
}
