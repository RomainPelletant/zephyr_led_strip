/**
 * @file ble_attrs.c
 * @author Romain PELLETANT (Kickmaker)
 * @date 30/09/2022
 * @copyright Copyright Zaack(c) 2022
 * @brief Manage BLE attributes parameters
 */
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <bt/bt.h>

#include <bt/bt_attrs.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bt_attrs, CONFIG_APP_LOG_LEVEL);

#define APPLY_CONFIG_VALUE 0x01

struct ble_values {
	// struct device_data dev_data;
	// struct config_data conf_data;
	// enum loader_network_interface interface_type_data;
	int toto;
};

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

static struct ble_values m_values;

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/**
 * @brief Load data to be read from BLE
 *
 * @param void
 * @return int 0 OK
 */
static void load_data(void);

/**
 * @brief Save data to be written from BLE
 *
 * @param void
 * @return int 0 OK
 */
static int save_data(void);

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static void load_data(void)
{
	/* Load device data */
	// const struct device_data *current_data = get_device_data();
	// memcpy(&m_values.dev_data, current_data, sizeof(struct device_data));

	// /* Load config data */
	// const struct config_data *current_config = get_config_data();
	// memcpy(&m_values.conf_data, current_config, sizeof(struct config_data));

}

static int save_data(void)
{
	int ret;
	// ret = config_data_set_enable_dhcp(m_values.conf_data.enable_dhcp);
	// ret |= config_data_set_static_ip(m_values.conf_data.static_ip);
	// ret |= config_data_set_netmask(m_values.conf_data.netmask);
	// ret |= config_data_set_gateway(m_values.conf_data.gateway);
	// ret |= config_data_set_dns(m_values.conf_data.dns);
	// ret |= config_data_set_ssid(m_values.conf_data.ssid);
	// ret |= config_data_set_password(m_values.conf_data.password);
	// ret |= config_data_set_wpa_method(m_values.conf_data.wpa_method);
	/* Please note that the client certificate is not stored here. Indeed,
	 * it is stored in the ESP32's internal memory directly upon writing
	 * the appropriate attribute. */

	if (ret != 0) {
		LOG_ERR("Failed to save data via BLE");
	}

	return ret;
}

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
void ble_attrs_init(void)
{
	load_data();
	return;
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t read_key_material(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset)
{
	// return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
	// 			 sizeof(struct key_material));
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_read_led_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
				 uint16_t len, uint16_t offset)
{
	LOG_INF("bt_attrs_read_led_mode");
	uint32_t bufd = 32U;
	buf = &bufd;
	len = sizeof(bufd);
	return sizeof(bufd);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &bufd,
				 sizeof(bufd));
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_write_led_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf,
			     uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_INF("bt_attrs_write_led_mode");
	// if (!buf || len == 0) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
	// }
	// if (offset + len > sizeof(m_values.conf_data.enable_dhcp)) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	// }

	// memcpy(&m_values.conf_data.enable_dhcp, buf, sizeof(m_values.conf_data.enable_dhcp));

	return len;
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_read_led_color(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			    uint16_t len, uint16_t offset)
{
	LOG_INF("bt_attrs_read_led_color");
	// return bt_gatt_attr_read(conn, attr, buf, len, offset, &m_values.conf_data.enable_dhcp,
	// 			 sizeof(m_values.conf_data.enable_dhcp));
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_write_led_color(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf,
			     uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_INF("bt_attrs_write_led_color");
	// if (!buf || len == 0) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
	// }
	// if (offset + len > sizeof(m_values.conf_data.enable_dhcp)) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	// }

	// memcpy(&m_values.conf_data.enable_dhcp, buf, sizeof(m_values.conf_data.enable_dhcp));

	return len;
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_read_led_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
				 uint16_t len, uint16_t offset)
{
	LOG_INF("bt_attrs_read_led_brightness");
	// return bt_gatt_attr_read(conn, attr, buf, len, offset, &m_values.conf_data.static_ip,
	// 			 strlen(m_values.conf_data.static_ip));
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_write_led_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				  const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_INF("bt_attrs_write_led_brightness");
	// if (!buf || len == 0) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
	// }
	// if (offset + len > sizeof(m_values.conf_data.static_ip)) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	// }

	// memset(&m_values.conf_data.static_ip, 0, sizeof(m_values.conf_data.static_ip));
	// memcpy(&m_values.conf_data.static_ip, buf, len);

	return len;
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_read_led_speed(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			      uint16_t len, uint16_t offset)
{
	LOG_INF("bt_attrs_read_led_speed");
	// return bt_gatt_attr_read(conn, attr, buf, len, offset, &m_values.conf_data.gateway,
	// 			 strlen(m_values.conf_data.gateway));
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_write_led_speed(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			       const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_INF("bt_attrs_write_led_speed");
	// if (!buf || len == 0) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
	// }
	// if (offset + len > sizeof(m_values.conf_data.gateway)) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	// }

	// memcpy(&m_values.conf_data.gateway, buf, len);

	return len;
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_read_device_state(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			       uint16_t len, uint16_t offset)
{
	LOG_INF("bt_attrs_read_device_state");
	// return bt_gatt_attr_read(conn, attr, buf, len, offset, &m_values.conf_data.netmask,
	// 			 strlen(m_values.conf_data.netmask));
}

// cppcheck-suppress [unusedFunction,unmatchedSuppression]
ssize_t bt_attrs_write_device_state(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_INF("bt_attrs_write_device_state");
	// if (!buf || len == 0) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
	// }
	// if (offset + len > sizeof(m_values.conf_data.netmask)) {
	// 	return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	// }

	// memset(&m_values.conf_data.netmask, 0, sizeof(m_values.conf_data.netmask));
	// memcpy(&m_values.conf_data.netmask, buf, len);

	return len;
}
