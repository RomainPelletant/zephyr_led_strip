#include <stdint.h>

#include <zephyr/kernel.h>

#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

#include <zephyr/bluetooth/ead.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/bluetooth.h>

#include <bt/bt_attrs.h>

#include <bt/bt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bt, CONFIG_APP_LOG_LEVEL);

/** @todo Zaack shall ask for identifier to BT sig */
#define MANUFACTURER_IDENTIFIER 0x0827

#define MANUFACTURER_ID_MSB ((MANUFACTURER_IDENTIFIER >> 8) & 0xff)
#define MANUFACTURER_ID_LSB ((MANUFACTURER_IDENTIFIER) & 0xff)

#define DEVICE_NAME     CONFIG_APP_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/////////////////////////////////////
// Local variables declarations
/////////////////////////////////////

static struct bt_conn_cb m_peripheral_cb;
static struct bt_conn_auth_cb m_peripheral_auth_cb;
static struct bt_gatt_exchange_params m_mtu_exchange_params_cb;
static struct bt_conn_auth_info_cb m_conn_auth_info_callbacks;

static struct bt_conn *m_conn;
struct bt_le_ext_adv *m_adv = NULL;

static struct k_poll_signal disconn_signal;
static struct k_poll_signal passkey_enter_signal;
static struct k_poll_signal pairing_status;

#if defined(CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING)

static struct bt_le_ext_adv_cb m_adv_cb;

static struct key_material mk = {
	.session_key = {0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB,
			0xCC, 0xCD, 0xCE, 0xCF},
	.iv = {0xFB, 0x56, 0xE1, 0xDA, 0xDC, 0x7E, 0xAD, 0xF5},
};

/** @struct encrypted advertisement data */
static const struct bt_data m_ad[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	BT_DATA_BYTES(BT_DATA_MANUFACTURER_DATA, MANUFACTURER_ID_LSB, MANUFACTURER_ID_MSB),
};

#else

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_SHORTENED, DEVICE_NAME, DEVICE_NAME_LEN),
};

#endif /* CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING */

/* Zaack Service */
static struct bt_uuid_128 zaack_ble_service_uuid =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe39, 0xcc7a, 0x482a, 0x984a, 0x7f2ed5b3e58f));

/* Network interface type */
static struct bt_uuid_128 led_mode_uuid =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe40, 0xcc7a, 0x482a, 0x984a, 0x7f2ed5b3e58f));

/* Enable DHCP*/
static struct bt_uuid_128 led_color_uuid =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe41, 0x8e22, 0x4541, 0x9d4c, 0x21edae82ed19));

/* Static IP address */
static struct bt_uuid_128 led_brightness_uuid =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe42, 0x8e22, 0x4541, 0x9d4c, 0x21edae82ed19));

/* Gateway IP address */
static struct bt_uuid_128 led_speed_uuid =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe43, 0x8e22, 0x4541, 0x9d4c, 0x21edae82ed19));

/* Gateway IP address */
static struct bt_uuid_128 device_state_uuid =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe43, 0x8e22, 0x4541, 0x9d4c, 0x21edae82ed19));

/* Zaack GATT services and characteristic */

BT_GATT_SERVICE_DEFINE(
	zaack_ble_svc, BT_GATT_PRIMARY_SERVICE(&zaack_ble_service_uuid),

#if defined(CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING)
	BT_GATT_CHARACTERISTIC(BT_UUID_GATT_EDKM, BT_GATT_CHRC_READ, BT_GATT_PERM_READ_AUTHEN,
			       read_key_material, NULL, &mk),
#endif /* CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING */

	/* Network interface type */
	BT_GATT_CHARACTERISTIC(&led_mode_uuid.uuid, BT_GATT_CHRC_READ  | BT_GATT_CHRC_WRITE,
				   BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN,
			       bt_attrs_read_led_mode, bt_attrs_write_led_mode, NULL),
	/* Enable DHCP*/
	BT_GATT_CHARACTERISTIC(&led_color_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN,
			       bt_attrs_read_led_color, bt_attrs_write_led_color, NULL),
	/* Static IP address */
	BT_GATT_CHARACTERISTIC(&led_brightness_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN,
			       bt_attrs_read_led_brightness, bt_attrs_write_led_brightness, NULL),
	/* Gateway IP address */
	BT_GATT_CHARACTERISTIC(&led_speed_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN,
			       bt_attrs_read_led_speed, bt_attrs_write_led_speed, NULL),
	/* Netmask IP address */
	BT_GATT_CHARACTERISTIC(&device_state_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN,
			       bt_attrs_read_device_state, bt_attrs_write_device_state, NULL));

/////////////////////////////////////
// Local function declarations
/////////////////////////////////////

/**
 * @brief Callback on MTU exchange done
 *
 * @param[inout] conn: BLE connection handler
 * @param[in] err: error passing
 * @param[inout] params: MTU negociation data structure
 * @return void
 */
static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err,
			    struct bt_gatt_exchange_params *params);

/**
 * @brief Request for MTU negociation
 * @details Default data size is 23 bytes. Request
 * a MTU negociation to increase this value.
 *
 * @param[inout] conn: BLE connection handler
 * @return int 0 OK
 */
static int mtu_exchange(struct bt_conn *conn);

#if defined(CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING)

/**
 * @brief Encrypt advertisement data
 * @details Encryption is based on struct key_material
 * in ble_attrs.h
 *
 * @param[inout] adv: BLE extended advertise handler
 * @return int 0 OK
 */
static int update_ad_data(struct bt_le_ext_adv *adv);

/**
 * @brief Set advertisement data
 *
 * @param[inout] adv: BLE extended advertise handler
 * @return int 0 OK
 */
static int set_ad_data(struct bt_le_ext_adv *adv);

/**
 * @brief Callback on random private address expiration
 *
 * @param[inout] adv: BLE extended advertise handler
 * @return int 0 OK
 */
static bool rpa_expired_cb(struct bt_le_ext_adv *adv);

/**
 * @brief Create advertisement data
 *
 * @param[inout] adv: BLE extended advertise handler
 * @return int 0 OK
 */
static int create_adv(struct bt_le_ext_adv **adv);

/**
 * @brief Start to advertise
 *
 * @param[inout] adv: BLE extended advertise handler
 * @return int 0 OK
 */
static int start_adv(struct bt_le_ext_adv *adv);

/**
 * @brief Stop and delete advertisement data
 *
 * @param[inout] adv: BLE extended advertise handler
 * @return int 0 OK
 */
static int stop_and_delete_adv(struct bt_le_ext_adv *adv);

#endif /* CONFIG_BLE_SECURITY_ENCRYPTED_ADVERTISING */

/**
 * @brief Callback on BLE connected
 *
 * @param[inout] conn: BLE connection handler
 * @param[in] err: connection error occured
 * @return void
 */
static void connected(struct bt_conn *conn, uint8_t err);

/**
 * @brief Callback on BLE disconnected
 *
 * @param[inout] disconn: connection handler
 * @param[in] reason: disconnection reason
 * @return void
 */
static void disconnected(struct bt_conn *disconn, uint8_t reason);

/**
 * @brief Callback on BLE paring failure
 *
 * @param[inout] conn: connection handler
 * @param[in] reason: pairing failure reason
 * @return void
 */
static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err);

/**
 * @brief Callback on passkey confirmation
 *
 * @param[inout] conn: connection handler
 * @param[in] passkey: random generated passkey
 * @return void
 */
static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey);

/**
 * @brief Callback to display generated passkey
 *
 * @param[inout] conn: connection handler
 * @param[in] passkey: random generated passkey
 * @return void
 */
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey);

/**
 * @brief Callback on authentication cancelation
 *
 * @param[inout] conn: connection handler
 * @return void
 */
static void auth_cancel(struct bt_conn *conn);

/**
 * @brief Callback on BLE paring failure
 *
 * @param[inout] conn: connection handler
 * @param[in] reason: pairing failure reason
 * @return void
 */
static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason);

/**
 * @brief Callback on BLE pairing success
 *
 * @param[inout] conn: connection handler
 * @param[in] bonded: bonded value (1 if OK)
 * @return void
 */
static void pairing_complete(struct bt_conn *conn, bool bonded);

/**
 * @brief Initialize BLE
 *
 * @return int 0 OK
 */
static int init_bt(void);

/**
 * @brief Confirm pairing passkey, shall be called on button pressed
 *
 * @param[inout] conn: connection handler
 * @return int 0 OK
 */
static int get_passkey_confirmation(struct bt_conn *conn);

/**
 * @brief Wait for a signal with timeout
 *
 * @return int 0 signal occured else timeout
 */
static int await_signal(struct k_poll_signal *sig, k_timeout_t timeout);

/**
 * @brief Wait for a signal with timeout and get result
 *
 * @return int result value or -ETIMEOUT if never occured
 */
static int await_signal_with_result(struct k_poll_signal *sig, k_timeout_t timeout);

/////////////////////////////////////
// Local functions definition
/////////////////////////////////////

static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err,
			    struct bt_gatt_exchange_params *params)
{
	LOG_DBG("%s: MTU exchange %s (%u)", __func__, err == 0U ? "successful" : "failed",
		bt_gatt_get_mtu(conn));
}

static int mtu_exchange(struct bt_conn *conn)
{
	int err;

	LOG_DBG("%s: Current MTU = %u", __func__, bt_gatt_get_mtu(conn));

	LOG_DBG("%s: Exchange MTU...", __func__);
	err = bt_gatt_exchange_mtu(conn, &m_mtu_exchange_params_cb);
	if (err) {
		LOG_ERR("%s: MTU exchange failed (err %d)", __func__, err);
	}

	return err;
}

#if defined(CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING)

static int update_ad_data(struct bt_le_ext_adv *adv)
{
	int err;

	/* Encrypt m_ad structure 1 */

	size_t size_ad_1 = BT_DATA_SERIALIZED_SIZE(m_ad[1].data_len);
	uint8_t ad_1[size_ad_1];
	size_t size_ead_1 = BT_EAD_ENCRYPTED_PAYLOAD_SIZE(size_ad_1);
	uint8_t ead_1[size_ead_1];

	bt_data_serialize(&m_ad[1], ad_1);

	err = bt_ead_encrypt(mk.session_key, mk.iv, ad_1, size_ad_1, ead_1);
	if (err != 0) {
		LOG_ERR("Error during first encryption");
		return -1;
	}

	/* Concatenate and update the advertising data */
	struct bt_data ad_structs[] = {
		m_ad[0],
		BT_DATA(BT_DATA_ENCRYPTED_AD_DATA, ead_1, size_ead_1),
	};

	LOG_INF("Advertising data size: %zu", bt_data_get_len(ad_structs, ARRAY_SIZE(ad_structs)));

	err = bt_le_ext_adv_set_data(adv, m_ad, ARRAY_SIZE(m_ad), NULL, 0);
	if (err) {
		LOG_ERR("Failed to set advertising data (%d)", err);
		return -1;
	}

	LOG_DBG("Advertising Data Updated");

	return 0;
}

static int set_ad_data(struct bt_le_ext_adv *adv)
{
	return update_ad_data(adv);
}

static bool rpa_expired_cb(struct bt_le_ext_adv *adv)
{
	LOG_DBG("RPA expired");

	/* The Bluetooth Core Specification say that the Randomizer and thus the
	 * Advertising Data shall be updated each time the address is changed.
	 *
	 * ref:
	 * Supplement to the Bluetooth Core Specification | v11, Part A 1.23.4
	 */
	update_ad_data(adv);

	return true;
}

static int create_adv(struct bt_le_ext_adv **adv)
{
	int err;
	struct bt_le_adv_param params;

	memset(&params, 0, sizeof(struct bt_le_adv_param));

	params.options |= BT_LE_ADV_OPT_CONNECTABLE;
	params.options |= BT_LE_ADV_OPT_EXT_ADV;

	params.id = BT_ID_DEFAULT;
	params.sid = 0;
	params.interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
	params.interval_max = BT_GAP_ADV_FAST_INT_MAX_2;

	m_adv_cb.rpa_expired = rpa_expired_cb;

	err = bt_le_ext_adv_create(&params, &m_adv_cb, adv);
	if (err) {
		LOG_ERR("Failed to create advertiser (%d)", err);
		return -1;
	}

	return 0;
}

static int start_adv(struct bt_le_ext_adv *adv)
{
	int err;
	int32_t timeout = 0;
	uint8_t num_events = 0;

	struct bt_le_ext_adv_start_param start_params;

	start_params.timeout = timeout;
	start_params.num_events = num_events;

	err = bt_le_ext_adv_start(adv, &start_params);
	if (err) {
		LOG_ERR("Failed to start advertiser (%d)", err);
		return -1;
	}

	LOG_DBG("Advertiser started");

	return 0;
}

static int stop_and_delete_adv(struct bt_le_ext_adv *adv)
{
	int err;

	err = bt_le_ext_adv_stop(adv);
	if (err) {
		LOG_ERR("Failed to stop advertiser (err %d)", err);
		return -1;
	}

	err = bt_le_ext_adv_delete(adv);
	if (err) {
		LOG_ERR("Failed to delete advertiser (err %d)", err);
		return -1;
	}

	return 0;
}

#endif /* CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING */

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		LOG_ERR("Failed to connect to %s (%u)", addr, err);
		return;
	}
	// (void)mtu_exchange(conn);

	LOG_INF("Connected to %s", addr);

	m_conn = conn;
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_DBG("Disconnected from %s (reason 0x%02x)", addr, reason);

	// k_poll_signal_raise(&disconn_signal, -ESHUTDOWN);
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		LOG_DBG("Security changed: %s level %u", addr, level);
	} else {
		LOG_DBG("Security failed: %s level %u err %d", addr, level, err);
	}
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	char passkey_str[7];
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	snprintk(passkey_str, ARRAY_SIZE(passkey_str), "%06u", passkey);

	LOG_INF("Passkey for %s: %s", addr, passkey_str);

	// k_poll_signal_raise(&passkey_enter_signal, 0);
}

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char passkey_str[7];
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	snprintk(passkey_str, ARRAY_SIZE(passkey_str), "%06u", passkey);

	LOG_DBG("Passkey for %s: %s", addr, passkey_str);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_DBG("Pairing cancelled: %s", addr);
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
	LOG_INF("Bonded %d", bonded);
	// k_poll_signal_raise(&pairing_status, 0);
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	LOG_INF("Failed %d", reason);
	// k_poll_signal_raise(&pairing_status, -ECONNREFUSED);
}

static void pairing_confirm(struct bt_conn *conn)
{
    bt_conn_auth_passkey_confirm(conn);
}

static int init_bt(void)
{
	int ret;

	// k_poll_signal_init(&disconn_signal);
	// k_poll_signal_init(&passkey_enter_signal);
	// k_poll_signal_init(&pairing_status);

	ret = bt_enable(NULL);
	if (ret) {
		LOG_ERR("Bluetooth init failed (err %d)", ret);
		return ret;
	}

	LOG_DBG("Bluetooth initialized");

	ret = bt_unpair(BT_ID_DEFAULT, BT_ADDR_LE_ANY);
	if (ret) {
		LOG_ERR("Unpairing failed (err %d)", ret);
	}

	/* Register BLE status callbacks */
	m_peripheral_cb.connected = connected;
	m_peripheral_cb.disconnected = disconnected;
	m_peripheral_cb.security_changed = security_changed;

	bt_conn_cb_register(&m_peripheral_cb);

	/* Register authetication callbacks */
	m_peripheral_auth_cb.pairing_confirm = pairing_confirm;
	m_peripheral_auth_cb.passkey_confirm = NULL;
	m_peripheral_auth_cb.passkey_display = NULL;
	m_peripheral_auth_cb.passkey_entry = NULL;
	m_peripheral_auth_cb.oob_data_request = NULL;
	m_peripheral_auth_cb.cancel = auth_cancel;

	ret = bt_conn_auth_cb_register(&m_peripheral_auth_cb);
	if (ret) {
		LOG_ERR("Failed to register bt_conn_auth_cb (err %d)", ret);
		return ret;
	}

	/* Register MTU exchange callback */
	// m_mtu_exchange_params_cb.func = mtu_exchange_cb;

	/* Register authetication information callbacks */
	m_conn_auth_info_callbacks.bond_deleted = NULL;
	m_conn_auth_info_callbacks.pairing_complete = pairing_complete;
	m_conn_auth_info_callbacks.pairing_failed = pairing_failed;

	(void)bt_conn_auth_info_cb_register(&m_conn_auth_info_callbacks);

	return 0;
}

static int get_passkey_confirmation(struct bt_conn *conn)
{
	int err;

	LOG_INF("Confirm passkey by pressing button at pin ...");

	/** @todo shall be managed by dedicated button GPIO */
	// await_signal(&button_pressed_signal);

	err = bt_conn_auth_passkey_confirm(conn);
	if (err) {
		LOG_DBG("Failed to confirm passkey.");
		return -1;
	}

	LOG_INF("Passkey confirmed.");

	return 0;
}

static int await_signal(struct k_poll_signal *sig, k_timeout_t timeout)
{
	struct k_poll_event events[] = {
		K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, sig),
	};

	return k_poll(events, ARRAY_SIZE(events), timeout);
}

static int await_signal_with_result(struct k_poll_signal *sig, k_timeout_t timeout)
{
	unsigned int event_signaled;
	int return_value;

	struct k_poll_event events[] = {
		K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, sig),
	};

	k_poll(events, ARRAY_SIZE(events), timeout);
	k_poll_signal_check(sig, &event_signaled, &return_value);

	/* If event occured and result is OK */
	if (event_signaled) {
		return return_value;
	}
	return -ETIMEDOUT;
}

/////////////////////////////////////
// Functions definition
/////////////////////////////////////

int ble_start(void)
{
	int ret;

	/* Load data to share from EEPROM */
	ble_attrs_init();

	if ((ret = init_bt()) != 0) {
		return ret;
	}

#if defined(CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING)
	/* Setup encrypted advertiser */
	if ((ret = create_adv(&m_adv)) != 0) {
		return ret;
	}

	if ((ret = start_adv(m_adv)) != 0) {
		return ret;
	}

	if ((ret = set_ad_data(m_adv)) != 0) {
		return ret;
	}
#else
	/* Start advertising */
	ret = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
	if (ret) {
		LOG_ERR("Advertising failed to start (ret %d)", ret);
		return ret;
	}
#endif /* CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING */

	return 0;
}

int ble_wait_for_pairing_attempt(k_timeout_t timeout)
{
	return await_signal(&passkey_enter_signal, timeout);
}

int ble_wait_for_pairing_done(k_timeout_t timeout)
{
	get_passkey_confirmation(m_conn);
	return await_signal_with_result(&pairing_status, timeout);
}

void configuration_completed(bool succeed)
{
	if (!succeed) {
		LOG_ERR("Failed saving configuration");
		k_poll_signal_raise(&disconn_signal, -EIO);
	}
	k_poll_signal_raise(&disconn_signal, 0);
}

int ble_wait_for_configuration_end(k_timeout_t timeout)
{
	return await_signal_with_result(&disconn_signal, timeout);
}

int ble_stop(void)
{
	/** @todo shall be remplaced by bt_disable()
	 * https://github.com/zephyrproject-rtos/zephyr/issues/52907
	 */

#if defined(CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING)
	if (!m_adv) {
		return -EFAULT;
	}
	return stop_and_delete_adv(m_adv);
#else
	return bt_le_adv_stop();
#endif /* CONFIG_APP_BT_SECURITY_ENCRYPTED_ADVERTISING */
}
