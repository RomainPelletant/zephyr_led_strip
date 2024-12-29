/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/sys/crc.h>

#include <context_storage/context_storage.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(context_storage, CONFIG_APP_LOG_LEVEL);

#define STORAGE_PARTITION storage_partition
#define STORAGE_DEVICE    FIXED_PARTITION_DEVICE(STORAGE_PARTITION)
#define STORAGE_AREA_ID   FIXED_PARTITION_ID(STORAGE_PARTITION)
#define STORAGE_SIZE      FIXED_PARTITION_SIZE(STORAGE_PARTITION)

#define FLASH_ERASE_BLOCK	4096U

#define STORAGE_FORMAT_REV    0x01

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/**
 * @brief Check if magic word is correct
 *
 * @param[inout] data:  pointer of struct context_data to be checked
 * @return bool true if OK / false if NOK
 */
static bool is_magic_number_valid(const struct context_data *data);

/**
 * @brief Check CRC value of context_data container structure
 *
 * @param[inout] data:  pointer of struct factory_data to be checked
 * @return bool true if OK / false if NOK
 */
static bool is_crc_valid(struct context_data *data);

static void set_default_values(struct context_data *data);

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static bool is_magic_number_valid(const struct context_data *data)
{
	if (strncmp(data->magic, CONTEXT_MAGIC_WORD, sizeof(CONTEXT_MAGIC_WORD)) == 0) {
		return true;
	}

	return false;
}

static bool is_crc_valid(struct context_data *data)
{
	uint32_t crc = crc32_ieee((void *)data, sizeof(struct context_data) - sizeof(uint32_t));
	if (crc != data->crc32) {
		LOG_ERR("CRC NOK");
		return false;
	}

	return true;
}

static void display_current_data(struct context_data *data)
{
	LOG_INF(" ------ [Context storage] ------");
	LOG_INF("| magic %s", data->magic);
	LOG_INF("| mode %d", data->mode);
	LOG_INF("| brightness %d", data->brightness);
	for (int i = 0; i < LED_PLAYER_MODE_MAX; i++) {
		LOG_INF("| pattern_context[%d]: %d", i, data->pattern_context[i]);
	}
	LOG_INF("| format_revision %d", data->format_revision);
	LOG_INF(" --------------------------------");
}

static void set_default_values(struct context_data *data)
{
	data->mode = LED_PLAYER_MODE_UNICOLOR_WHITE;
	data->brightness = 50U;
}

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

int context_storage_read(struct context_data *data)
{
	const struct flash_area *flash_area;

	if (!data) {
		return -EFAULT;
	}

	set_default_values(data);

	if (flash_area_open(STORAGE_AREA_ID, &flash_area) != 0) {
		LOG_ERR("Failed to open eeprom");
		flash_area_close(flash_area);
		return -ENODEV;
	}

	if (flash_area_read(flash_area, 0, data, sizeof(struct context_data)) != 0) {
		LOG_ERR("Failed to read EEPROM data config");
		flash_area_close(flash_area);
		return -EIO;
	}

	/* Check magic word for data comissioning check */
	if (!is_magic_number_valid(data)) {
		LOG_ERR("Wrong magic number");
		flash_area_close(flash_area);
		return -ENODATA;
	}

	display_current_data(data);
	/* Check CRC value for data integrity check */
	if (!is_crc_valid(data)) {
		LOG_ERR("Failed to check CRC data device");
		flash_area_close(flash_area);
		return -EBADMSG;
	}

	flash_area_close(flash_area);

	return 0;
}

int context_storage_write(struct context_data *data)
{
	const struct flash_area *flash_area;
	uint32_t crc;

	if (flash_area_open(STORAGE_AREA_ID, &flash_area) != 0) {
		LOG_ERR("Failed to open eeprom");
		flash_area_close(flash_area);
		return -ENODEV;
	}

	strncpy(data->magic, CONTEXT_MAGIC_WORD, sizeof(CONTEXT_MAGIC_WORD));

	/* Set current format revision */
	data->format_revision = STORAGE_FORMAT_REV;

	/* Compute CRC */
	crc = crc32_ieee((void *)data, sizeof(struct context_data) - sizeof(uint32_t));
	data->crc32 = crc;

	if (flash_area_erase(flash_area, 0, FLASH_ERASE_BLOCK) != 0) {
		LOG_ERR("Failed to write EEPROM data device");
		flash_area_close(flash_area);
		return -EIO;
	}

	if (flash_area_write(flash_area, 0, data, sizeof(struct context_data)) != 0) {
		LOG_ERR("Failed to write EEPROM data device");
		flash_area_close(flash_area);
		return -EIO;
	}

	display_current_data(data);

	flash_area_close(flash_area);

	return 0;
}
