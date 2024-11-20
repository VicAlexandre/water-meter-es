/**
 * @brief Defines the HSCR-04 driver module.
 * @author Victor Miranda <varm@ic.ufal.br>
 * @date 01-11-2024
 */

#ifndef HCSR04_H
#define HCSR04_H

#include <zephyr/kernel.h>
#include <errno.h>
#include <stdint.h>

struct hcsr04_data {
  uint32_t distance;
  k_timepoint_t timestamp;
};

/**
 * @brief Initializes the HCSR-04 driver and spawns its thread.
 */
int hcsr04_init(void);

/**
 * @brief Reads the distance measured by the HCSR-04 sensor.
 * @param[out] distance The distance measured by the sensor.
 * @return 0 on success, -ERRNO otherwise.
 */
int hcsr04_read_distance(uint32_t *distance);

#endif /* HCSR04_H */