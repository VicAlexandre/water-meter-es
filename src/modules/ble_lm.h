/**
 * @brief Defines the Bluetooth Low Energy (BLE) module.
 * @author Victor Miranda <varm@ic.ufal.br>
 * @date 01-11-2024
 */

#ifndef LM_BLE_H
#define LM_BLE_H

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/types.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/bluetooth/uuid.h>

#include "hcsr04.h"

/**
 * @brief Initializes the BLE module.
 * @return 0 on success, -ERRNO otherwise.
 */
int ble_init(void);

/**
 * @brief Sends an alarm through BLE.
 * @param[in] data The data to be sent.
 * @return 0 if the alarm was sent successfuly, -ERRNO otherwise.
 */
int ble_send_alarm(struct hcsr04_data *data);

/**
 * @brief Sends a critical alarm through BLE.
 * @param[in] data The data to be sent.
 * @return 0 if the critical alarm was sent successfuly, -ERRNO otherwise.
 */
int ble_send_critical_alarm(struct hcsr04_data *data);

#endif /* LM_BLE_H */
