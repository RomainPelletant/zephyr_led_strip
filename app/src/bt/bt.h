 /*
 * Copyright (c) 2022 Romain PELLETANT
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BT_H
#define BT_H

#include <zephyr/bluetooth/ead.h>

struct key_material {
	uint8_t session_key[BT_EAD_KEY_SIZE];
	uint8_t iv[BT_EAD_IV_SIZE];
} __packed;

/**
 * @brief Init BLE
 *
 * @param void
 * @return int 0 OK
 */
int ble_start(void);

/**
 * @brief Wait for device to receive boding attempt/connexion
 * @details function returns when the device has been requested
 * for connexion attempt until timeout
 *
 * @param[in] timeout: max time to wait
 * @return int 0 OK else timeout
 */
int ble_wait_for_pairing_attempt(k_timeout_t timeout);

/**
 * @brief Wait for device to be bonded by pressing button
 * @details function returns when the device has been
 * successfully bonded on a device until timeout
 *
 * @param[in] timeout: max time to wait
 * @return int 0 OK else timeout
 */
int ble_wait_for_pairing_done(k_timeout_t timeout);

/**
 * @brief Notify configuration is ended
 *
 * @param[in] succeed: configuration save flag
 * @return void
 */
void configuration_completed(bool succeed);

/**
 * @brief Wait for the configuration to be ended
 * and saved
 *
 * @param[in] timeout: max time to wait
 * @return int 0 OK else timeout
 */
int ble_wait_for_configuration_end(k_timeout_t timeout);

/**
 * @brief Stop BLE
 *
 * @param void
 * @return int 0 OK
 */
int ble_stop(void);

#endif /* BLE_H */