/**
 * @brief Implementation of the data manager module.
 * @author Sandro Gomes <sgp@ic.ufal.br>
 * @date 16-11-2024
 */
#include "data_manager.h"
#include "hcsr04.h"

#include <zephyr/kernel.h>
#include <stdint.h>
#include <zephyr/logging/log.h>

#define READER_PRIORITY 2 /**< Priority level for the data reader thread. */

static bool initialized = false; /**< Flag indicating whether the module is initialized. */

static uint8_t *dynamic_buffer = NULL; /**< Dynamic buffer to store data polled from the sensor. */

static struct data_man_conf data_man_conf; /**< Configuration structure for data manager. */

static struct k_thread reader_tid; /**< Thread structure for data reader. */

static bool alarm_triggered = false; /**< Flag indicating whether an alarm was triggered. */

static bool critical_alarm_triggered =
	false; /**< Flag indicating whether a critical alarm was triggered. */

/* Timer handlers and definitions */
void data_reader_timer_handler(struct k_timer *dummy);

K_THREAD_STACK_DEFINE(reader_stack, 1024); /**< Thread stack for data reader. */

K_TIMER_DEFINE(data_man_timer, data_reader_timer_handler, NULL);

LOG_MODULE_REGISTER(data_manager);

/**
 * @brief Timer handler for data reading. Polls data from the sensor at regular intervals.
 *
 * @param dummy Unused parameter.
 */
void data_reader_timer_handler(struct k_timer *dummy)
{
	int ret = data_man_poll();

	if (ret != 0) {
		LOG_ERR("Erro ao obter dados do sensor: %d", ret);
	}
}

/**
 * @brief Thread function for data reading.
 *
 * @param arg1 Pointer to the configuration structure.
 * @param arg2 Unused parameter.
 * @param arg3 Unused parameter.
 */
void data_reader_thread(void *arg1, void *arg2, void *arg3)
{
	struct data_man_conf *config = (struct data_man_conf *)arg1;

	k_timer_start(&data_man_timer, config->poll_interval, config->poll_interval);

	return;
}

int data_man_init(uint8_t buff_size, struct data_man_conf *config)
{
	if (initialized) {
		return -EAGAIN;
	}

	dynamic_buffer = k_malloc(buff_size);
	if (dynamic_buffer == NULL) {
		return -ENOMEM;
	}

	ring_buf_init(&config->buffer, buff_size, dynamic_buffer);

	data_man_conf = *config;

	initialized = true;

	LOG_INF("data_manager inicializado com sucesso.");

	k_thread_create(&reader_tid, reader_stack, K_THREAD_STACK_SIZEOF(reader_stack),
			data_reader_thread, &data_man_conf, NULL, NULL, READER_PRIORITY, 0,
			K_NO_WAIT);

	return 0;
}

int data_man_poll(void)
{
	struct hcsr04_data data;
	int ret;

	ret = hcsr04_read_distance(&data);
	if (ret != 0) {
		return ret;
	}
	LOG_INF("Distance read: %d", data.distance);
	size_t len = sizeof(data);

	if (ring_buf_space_get(&data_man_conf.buffer) < len) {
		ring_buf_get(&data_man_conf.buffer, NULL, len);
	}

	ret = ring_buf_put(&data_man_conf.buffer, (uint8_t *)&data, len);
	if (ret < 0) {
		return -EINVAL;
	}

	alarm_triggered = data.distance <= data_man_conf.alarm_threshold ? true : false;

	critical_alarm_triggered =
		data.distance <= data_man_conf.critical_alarm_threshold ? true : false;

	return 0;
}

int data_man_get(struct hcsr04_data *data)
{
	size_t len = sizeof(*data);

	if (data == NULL) {
		return -EINVAL;
	}

	size_t last_position = (data_man_conf.buffer.put_head - len) % data_man_conf.buffer.size;

	memcpy(data, &data_man_conf.buffer.buffer[last_position], len);

	return 0;
}

int data_man_get_bulk(struct hcsr04_data *data, uint8_t n)
{
	if (data == NULL || n == 0) {
		return -EINVAL;
	}

	size_t len = sizeof(*data);
	size_t total_data_size = n * len;

	if (ring_buf_is_empty(&data_man_conf.buffer) ||
	    data_man_conf.buffer.size < total_data_size) {
		return -EINVAL;
	}

	size_t tail_pos = data_man_conf.buffer.get_tail;

	for (uint8_t i = 0; i < n; i++) {
		size_t pos = (tail_pos + data_man_conf.buffer.size - ((i + 1) * len)) %
			     data_man_conf.buffer.size;
		memcpy(&data[i], data_man_conf.buffer.buffer + pos, len);
	}

	return 0;
}

int data_man_get_config(struct data_man_conf *config)
{
	if (config == NULL) {
		return -EINVAL;
	}

	*config = data_man_conf;

	return 0;
}

int data_man_set_config(struct data_man_conf *config)
{
	if (config == NULL) {
		return -EINVAL;
	}

	data_man_conf = *config;

	return 0;
}

int data_man_get_alarm_triggered(bool *alarm_trigg)
{
	if (alarm_trigg == NULL) {
		return -EFAULT;
	}

	*alarm_trigg = alarm_triggered;

	return 0;
}

int data_man_get_critical_alarm_triggered(bool *critical_alarm_trigg)
{
	if (critical_alarm_trigg == NULL) {
		return -EFAULT;
	}

	*critical_alarm_trigg = critical_alarm_triggered;

	return 0;
}

int data_man_deinit(void)
{
	if (dynamic_buffer != NULL) {
		k_free(dynamic_buffer);
		dynamic_buffer = NULL;
	}

	return 0;
}