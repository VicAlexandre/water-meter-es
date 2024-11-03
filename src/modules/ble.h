/**
 * @brief Defines the Bluetooth Low Energy (BLE) module.
 * @author Victor Miranda <varm@ic.ufal.br>
 * @date 01-11-2024
 */

#ifndef BLE_H
#define BLE_H

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

#endif /* BLE_H */
