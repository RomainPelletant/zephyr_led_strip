/*
 * Copyright (c) 2024 Romain Pelletant
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CONTEXT_STORAGE_H
#define CONTEXT_STORAGE_H

#include <led_player/led_player.h>

#define CONTEXT_MAGIC_WORD "MAGICUCU"
#define CONTEXT_PATTERN_CONTEXT_MAX_SIZE	4U

struct context_data {
	char magic[sizeof(CONTEXT_MAGIC_WORD)];
	uint32_t mode;
    uint32_t brightness;
	uint32_t pattern_context[LED_PLAYER_MODE_MAX];
	uint32_t format_revision;
	uint32_t crc32;
} __attribute__((packed));

/**
 * @brief Read config data
 * @details Read EEPROM and copy values into static memory
 *
 * @return int 0 OK else negative
 */
int context_storage_read(struct context_data *data);

/**
 * @brief Write config data
 * @details Write congif_data into EEPROM
 *
 * @param[inout] data:  pointer of struct context_data to be written
 * @return int 0 OK else negative
 */
int context_storage_write(struct context_data *data);

#endif /* CONTEXT_STORAGE_H */