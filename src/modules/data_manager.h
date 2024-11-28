/**
 * @brief Defines the data manager module.
 * @author Victor Miranda <varm@ic.ufal.br>
 * @date 01-11-2024
 */
#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "hcsr04.h"

#include <zephyr/kernel.h>
#include <stdint.h>
#include <zephyr/sys/ring_buffer.h>

/**
 * @brief Stores a configuration for the data manager module.
 */
struct data_man_conf {
	k_timeout_t poll_interval;         /* The interval between each poll of the sensors. */
	uint32_t alarm_threshold;          /* The threshold to trigger an alarm. */
	uint32_t critical_alarm_threshold; /* The threshold to trigger a critical alarm. */
	struct ring_buf buffer;            /* The circular buffer. */
};

/**
 * @brief Initializes the data manager module and spawns its thread.
 * @param buff_size The size of the buffer to store data polled from the sensors.
 * @param config The configuration for the data manager.
 * @return 0 on success, -ERRNO otherwise.
 */
int data_man_init(uint8_t buff_size, struct data_man_conf *config);

/**
 * @brief Forces the manager to poll the sensors and stores the data in the buffer.
 * @return 0 on success, -ERRNO otherwise.
 */
int data_man_poll(void);

/**
 * @brief Gets the last data polled by the manager.
 * @param[out] data The data to be returned.
 * @return 0 on success, -ERRNO otherwise.
 */
int data_man_get(struct hcsr04_data *data);

/**
 * @brief Gets the last n data polled by the manager.
 * @param[out] data The data to be returned.
 * @param n The number of data to be returned.
 * @return 0 on success, -EINVAL if n is greater than the buffer size, -ERRNO otherwise.
 */
int data_man_get_bulk(struct hcsr04_data *data, uint8_t n);

/**
 * @brief Gets the configuration of the data manager.
 * @param[out] config The configuration to be returned.
 * @return 0 on success, -ERRNO otherwise.
 */
int data_man_get_config(struct data_man_conf *config);

/**
 * @brief Sets the configuration of the data manager.
 * @param config The configuration to be set.
 * @return 0 on success, -ERRNO otherwise
 */
int data_man_set_config(struct data_man_conf *config);

int data_man_get_alarm_triggered(bool *alarm_trigg);

int data_man_get_critical_alarm_triggered(bool *critical_alarm_trigg);

/**
 * @brief Deinitializes the data manager module.
 * @return 0 on success, -ERRNO otherwise.
 */
int data_man_deinit(void);

#endif /* DATA_MANAGER_H */