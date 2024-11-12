/**
 * @brief Defines the Bluetooth Low Energy (BLE) module.
 * @author Victor Miranda <varm@ic.ufal.br>
 * @date 01-11-2024
 */

#include "ble_lm.h"
#include "guards.h"

#define SERVICE_UUID              0X0222 /* Generic BLE service UUID. */
#define LATEST_READING_UUID       0X0001 /* Latest level reading UUID. */
#define LEVEL_ALARM_UUID          0X0002 /* Level alarm UUID. */
#define CRITICAL_LEVEL_ALARM_UUID 0X0003 /* Critical level alarm UUID. */

#define BLE_PASSKEY 123654 /* BLE Generic Passkey */

/**
 * @brief Structure that holds the fields used in the BLE module.
 */
static struct ble_fields {
	bool is_init;              /**< Flag that indicates the initialization of this module. */
	struct bt_uuid_16 service; /**< UUID of the service. */
	struct bt_uuid_16 latest_reading;       /**<  UUID of the latest level reading. */
	struct bt_uuid_16 level_alarm;          /**<  UUID of the level alarm. */
	struct bt_uuid_16 critical_level_alarm; /**< UUID of the critical level alarm. */
	struct bt_conn *conn;                   /**< Bluetooth connection obj. */
	uint8_t conn_error;                     /**< Connection error flag. */
	uint8_t disconnection_reason;           /**< Disconnection reason flag. */
	uint8_t ad_len;                         /**< Bluetooth advertising data array len */
	struct bt_data ad[];                    /**<  Bluetooth advertising data array. */
} self = {
	.is_init = false,
	.service = BT_UUID_INIT_16(SERVICE_UUID),
	.latest_reading = BT_UUID_INIT_16(LATEST_READING_UUID),
	.level_alarm = BT_UUID_INIT_16(LEVEL_ALARM_UUID),
	.critical_level_alarm = BT_UUID_INIT_16(CRITICAL_LEVEL_ALARM_UUID),
	.ad_len = 0,
};

/**
 * @brief BLE connection callback.
 *
 * @param conn Connection object.
 * @param err Error code.
 */
static void connected(struct bt_conn *conn, uint8_t err);

/**
 * @brief BLE disconnection callback.
 *
 * @param conn Connection object.
 * @param reason Disconnection reason.
 */
static void disconnected(struct bt_conn *conn, uint8_t reason);

/**
 * @brief Read the latest reading from the sensor.
 *
 * @param conn Connection object.
 * @param attr Attribute object.
 * @param buf Buffer to store the reading.
 * @param len Buffer length.
 * @param offset Buffer offset.
 * @return ssize_t The size of the message.
 */
static ssize_t read_latest_reading(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
				   uint16_t len, uint16_t offset);

/**
 * @brief Read the alarm status.
 *
 * @param conn Connection object.
 * @param attr Attribute object.
 * @param buf Buffer to store the reading.
 * @param len Buffer length.
 * @param offset Buffer offset.
 * @return ssize_t The size of the message.
 */
static ssize_t read_alarm(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset);

/**
 * @brief Read the critical alarm status.
 *
 * @param conn Connection object.
 * @param attr Attribute object.
 * @param buf Buffer to store the reading.
 * @param len Buffer length.
 * @param offset Buffer offset.
 * @return ssize_t The size of the message.
 */
static ssize_t read_critical_alarm(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
				   uint16_t len, uint16_t offset);

/* Connection callbacks definition */
BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

/* Unused authentication callbacks */
static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = NULL,
	.passkey_entry = NULL,
	.cancel = NULL,
	.pairing_confirm = NULL,
};

/* Bluetooth service definition */
BT_GATT_SERVICE_DEFINE(lm_svc, BT_GATT_PRIMARY_SERVICE(&self.service),
		       BT_GATT_CHARACTERISTIC(&self.latest_reading.uuid, BT_GATT_CHRC_READ,
					      BT_GATT_PERM_READ_AUTHEN, read_latest_reading, NULL,
					      NULL),
		       BT_GATT_CUD("Leitura de Nível", BT_GATT_PERM_READ),
		       BT_GATT_CHARACTERISTIC(&self.level_alarm.uuid, BT_GATT_CHRC_READ,
					      BT_GATT_PERM_READ_AUTHEN, read_alarm, NULL, NULL),
		       BT_GATT_CUD("Alarme de Nível Baixo", BT_GATT_PERM_READ),
		       BT_GATT_CHARACTERISTIC(&self.critical_level_alarm.uuid, BT_GATT_CHRC_READ,
					      BT_GATT_PERM_READ_AUTHEN, read_critical_alarm, NULL,
					      NULL),
		       BT_GATT_CUD("Alarme Crítico de Nível Baixo", BT_GATT_PERM_READ), );

int ble_init(void)
{
	LM_CHECK_ERROR(bt_enable(NULL));
	LM_CHECK_ERROR(bt_le_adv_start(BT_LE_ADV_CONN_NAME, self.ad, self.ad_len, NULL, 0));

	LM_CHECK_ERROR(bt_passkey_set(BLE_PASSKEY));

	LM_CHECK_ERROR(bt_conn_auth_cb_register(&auth_cb_display));

	self.is_init = true;

	return 0;
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	self.conn = conn;
	self.conn_error = err;
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	self.disconnection_reason = reason;
	self.conn = NULL;

	bt_unpair(BT_ID_DEFAULT, NULL);
}

static ssize_t read_latest_reading(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
				   uint16_t len, uint16_t offset)
{
	ssize_t msg;
	uint32_t distance_reading = 0;

	msg = bt_gatt_attr_read(conn, attr, buf, len, offset, &distance_reading,
				sizeof(distance_reading));

	return msg;
}

static ssize_t read_alarm(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset)
{
	ssize_t msg;
	uint32_t alarm = 0;

	msg = bt_gatt_attr_read(conn, attr, buf, len, offset, &alarm, sizeof(alarm));

	return msg;
}

static ssize_t read_critical_alarm(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
				   uint16_t len, uint16_t offset)
{
	ssize_t msg;
	uint32_t critical_alarm = 0;

	msg = bt_gatt_attr_read(conn, attr, buf, len, offset, &critical_alarm,
				sizeof(critical_alarm));

	return msg;
}