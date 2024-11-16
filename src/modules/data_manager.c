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

LOG_MODULE_REGISTER(data_manager);

#define READER_PRIORITY 2                   /**< Priority level for the data reader thread. */

static bool initialized = false;            /**< Flag indicating whether the module is initialized. */
static uint8_t buffer_index = 0;            /**< Index to keep track of the current position in the buffer. */

struct data_man_conf data_man_conf;         /**< Configuration structure for data manager. */

static struct k_thread reader_tid;          /**< Thread structure for data reader. */
K_THREAD_STACK_DEFINE(reader_stack, 1024);  /**< Thread stack for data reader. */   

/* Timer handlers and definitions */
void data_reader_timer_handler(struct k_timer *dummy);
K_TIMER_DEFINE(data_man_timer, data_reader_timer_handler, NULL);

/**
 * @brief Timer handler for data reading. Polls data from the sensor at regular intervals.
 * 
 * @param dummy Unused parameter.
 */
void data_reader_timer_handler(struct k_timer *dummy) {
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
void data_reader_thread(void *arg1, void *arg2, void *arg3) {
    struct data_man_conf *config = (struct data_man_conf *)arg1;

    k_timer_start(&data_man_timer, config->poll_interval, config->poll_interval);

    while (1) {
        k_sleep(K_FOREVER);
    }
}

int data_man_init(uint8_t buff_size, struct data_man_conf *config) {
    if (initialized) {
        return -EAGAIN; 
    }

    config->buffer_data = k_malloc(buff_size);
    if (config->buffer_data == NULL) {
        return -ENOMEM;
    }
    config->buffer_size = buff_size;   
 
    data_man_conf = *config;

    initialized = true;
    LOG_INF("data_manager inicializado com sucesso.");

    k_thread_create(&reader_tid, reader_stack, K_THREAD_STACK_SIZEOF(reader_stack),
                    data_reader_thread, &data_man_conf, NULL, NULL, READER_PRIORITY, 0, K_NO_WAIT);
    
    return 0;
}

int data_man_poll(void) {
    struct hcsr04_data data;
    int ret;

    ret = hcsr04_read_distance(&data.distance); 
    if (ret < 0) {
        return ret; 
    }

    data_man_conf.buffer_data[buffer_index] = data;
    buffer_index = (buffer_index + 1) % data_man_conf.buffer_size;

    return 0;
}

int data_man_get(struct hcsr04_data *data) {
    if (data == NULL) {
        return -EINVAL;
    }

    *data = data_man_conf.buffer_data[(buffer_index - 1 + data_man_conf.buffer_size) % data_man_conf.buffer_size];

    if (data->distance == 0) {
        return -EIO;
    }

    return 0;
}

int data_man_get_bulk(struct hcsr04_data *data, uint8_t n) {
    size_t len = sizeof(*data) * n;

    if (data_man_conf.buffer_size < len) {
        return -EINVAL;
    }

    for (uint8_t i = 0; i < n; i++) {
        int idx = (buffer_index - 1 - i + data_man_conf.buffer_size) % data_man_conf.buffer_size;
        data[i] = data_man_conf.buffer_data[idx];
    }

    for (uint8_t i = 0; i < n; i++) {
        if (data[i].distance == 0) {
            return -EIO;
        }
    }

    return 0;
}

int data_man_get_config(struct data_man_conf *config) {
    if (config == NULL) {
        return -EINVAL; 
    }

    *config = data_man_conf;
    
    return 0; 
}

int data_man_set_config(struct data_man_conf *config) {
    if (config == NULL) {
        return -EINVAL; 
    }

    data_man_conf = *config;
    
    return 0; 
}

int data_man_deinit(void){
    if (data_man_conf.buffer_data != NULL) {
        k_free(data_man_conf.buffer_data); 
        data_man_conf.buffer_data = NULL;   
        return 0;                 
    }
    
    return -EINVAL; 
}