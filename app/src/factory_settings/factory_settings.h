/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FACTORY_SETTINGS_H
#define FACTORY_SETTINGS_H

#define FACTORY_MAGIC_WORD "MAGICOCO"

struct factory_data {
	char magic[sizeof(FACTORY_MAGIC_WORD)];
	uint32_t led_length;
    uint32_t rgbw;
	uint32_t format_revision;
	uint32_t crc32;
} __attribute__((packed));

/**
 * @brief Read config data
 * @details Read EEPROM and copy values into static memory
 *
 * @return int 0 OK else negative
 */
int factory_settings_read(void);

/**
 * @brief Write config data
 * @details Write congif_data into EEPROM
 *
 * @param[inout] data:  pointer of struct config_data to be written
 * @return int 0 OK else negative
 */
int factory_settings_write(struct factory_data *data);

/**
 * @brief Get current config data
 * @warning not thread-safe
 *
 * @return struct config_data (only read access)
 */

struct factory_data *factory_settings_get(void);

#endif /* FACTORY_SETTINGS_H */