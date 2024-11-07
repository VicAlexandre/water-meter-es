/**
 * @brief Defines the Bluetooth Low Energy (BLE) module.
 * @author Victor Miranda <varm@ic.ufal.br>
 * @date 01-11-2024
 */

#include "ble_lm.h"
#include "guards.h"

#define SERVICE_UUID              0X0000
#define LATEST_READING_UUID       0X0001
#define LEVEL_ALARM_UUID          0X0002
#define CRITICAL_LEVEL_ALARM_UUID 0X0003

#define BLE_PASSKEY 123654

/**
 * @brief Structure that holds the fields used in the BLE module.
 */
static struct ble_fields {
	bool is_init;              /**< Flag that indicates the initialization of this module. */
	struct bt_uuid_16 service; /**< UUID of the service. */
	struct bt_uuid_16 latest_reading;       /**<  UUID of the latest level reading. */
	struct bt_uuid_16 level_alarm;          /**<  UUID of the level alarm. */
	struct bt_uuid_16 critical_level_alarm; /**< UUID of the critical level alarm. */
	struct bt_data ad[];                    /**<  Bluetooth advertising data array. */
	uint8_t ad_len;                         /**< Bluetooth advertising data array len */
} self = {
	.is_init = false,
	.service = BT_UUID_INIT_16(SERVICE_UUID),
	.latest_reading = BT_UUID_INIT_16(LATEST_READING_UUID),
	.level_alarm = BT_UUID_INIT_16(LEVEL_ALARM_UUID),
	.critical_level_alarm = BT_UUID_INIT_16(CRITICAL_LEVEL_ALARM_UUID),
	.ad_len = 0,
};

int ble_init(void)
{
	LM_CHECK_ERROR(bt_enable(NULL));
	LM_CHECK_ERROR(bt_le_adv_start(BT_LE_ADV_CONN_NAME, self.ad, self.ad_len, NULL, 0));

	LM_CHECK_ERROR(bt_passkey_set(BLE_PASSKEY));

	self.is_init = true;

	return 0;
}