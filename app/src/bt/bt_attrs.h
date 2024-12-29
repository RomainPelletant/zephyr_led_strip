 /*
 * Copyright (c) 2022 Romain PELLETANT
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/bluetooth/ead.h>

#ifndef BT_ATTRS_H
#define BT_ATTRS_H

/**
 * @brief Init BLE parameters data
 *
 * @param void
 * @return int 0 OK
 */
void ble_attrs_init(void);

/**
 * @brief Read key material value
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer for read
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t read_key_material(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset);

/**
 * @brief Read network interface value
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer for read
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t bt_attrs_read_led_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
				 uint16_t len, uint16_t offset);

ssize_t bt_attrs_write_led_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf,
			     uint16_t len, uint16_t offset, uint8_t flags);

/**
 * @brief Read DHCP activation value
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer for read
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t bt_attrs_read_led_color(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			    uint16_t len, uint16_t offset);

/**
 * @brief Write DHCP activation value
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer to write
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @param[in] flags: write flags related
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t bt_attrs_write_led_color(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf,
			     uint16_t len, uint16_t offset, uint8_t flags);

/**
 * @brief Read static IP address
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer for read
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t bt_attrs_read_led_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
				 uint16_t len, uint16_t offset);

/**
 * @brief Write static IP address
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer to write
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @param[in] flags: write flags related
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t bt_attrs_write_led_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				  const void *buf, uint16_t len, uint16_t offset, uint8_t flags);

/**
 * @brief Read gateway IP address
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer for read
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t bt_attrs_read_led_speed(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			      uint16_t len, uint16_t offset);

/**
 * @brief Write gateway IP address
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer to write
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @param[in] flags: write flags related
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t bt_attrs_write_led_speed(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			       const void *buf, uint16_t len, uint16_t offset, uint8_t flags);

/**
 * @brief Read netmask IP address
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer for read
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t bt_attrs_read_device_state(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			       uint16_t len, uint16_t offset);

/**
 * @brief Write netmask IP address
 *
 * @param[inout] conn: connection handler
 * @param[inout] attr: gatt parameter attributes pointer
 * @param[inout] buf: data pointer to write
 * @param[in] len: length of data to read
 * @param[in] offset: offset to start reading from
 * @param[in] flags: write flags related
 * @return number fo bytes read if OK, BT_GATT_ERR NOK
 */
ssize_t bt_attrs_write_device_state(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				const void *buf, uint16_t len, uint16_t offset, uint8_t flags);

#endif /* BT_ATTRS_H */