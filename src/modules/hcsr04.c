#include "hcsr04.h"
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY 2
#define DELAY 100

#define GPIO_DEVICE   "GPIO_0"
#define GPIO_TRIG_PIN 13
#define GPIO_ECHO_PIN 12

/**
 * @brief Blocks the current thread calculating the distance every DELAY millis.
 */
void calculate_distance(void *p1, void *p2, void *p3);

/**
 * @brief Rounds the given floating point number to the nearest integer.
 * @param[in] n The number to round.
 * @return The number rounded.
 */
uint32_t round_to_int(double n);

K_THREAD_STACK_DEFINE(hcsr04_stack_area, THREAD_STACK_SIZE);
struct k_thread thread_data;

struct device *dev;

uint32_t distance_cm = -1; // last measured distance in cm

int hcsr04_init(void)
{
    // setup device
    dev = device_get_binding(GPIO_DEVICE);
    if (dev == NULL) return -EINVAL;

    int ret;

    ret = gpio_pin_configure(dev, GPIO_TRIG_PIN, GPIO_OUTPUT);
    if (ret < 0) return ret;

    ret = gpio_pin_configure(dev, GPIO_ECHO_PIN, GPIO_INPUT);
    if (ret < 0) return ret;

    k_thread_create(&thread_data, hcsr04_stack_area,
                    K_THREAD_STACK_SIZEOF(hcsr04_stack_area),
                    calculate_distance,
                    NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    return 0;
}

int hcsr04_read_distance(uint32_t *distance)
{
    if (distance_cm < 0) {
        return -EINVAL;
    }
    *distance = distance_cm;
    return 0;
}

void calculate_distance(void *p1, void *p2, void *p3)
{
    while (1) {
        // fire ultrasonic burst
        gpio_pin_set(dev, GPIO_TRIG_PIN, 1);
        k_usleep(10);
        gpio_pin_set(dev, GPIO_TRIG_PIN, 0);

        int echo_val;

        // wait for HIGH
        do {
            echo_val = gpio_pin_get(dev, GPIO_ECHO_PIN);
        } while (echo_val == 0);

        uint32_t start = k_cycle_get_32();

        // wait for LOW
        do {
            echo_val = gpio_pin_get(dev, GPIO_ECHO_PIN);
        } while (echo_val == 1);

        // calculate distance
        uint32_t cycles = k_cycle_get_32() - start;
        double seconds = (double) cycles / sys_clock_hw_cycles_per_sec();

        double distance_m = (seconds * 343) / 2; // speed of sound: 343 m/s
        distance_cm = round_to_int(distance_m / 100);

        // apply delay
        k_sleep(K_MSEC(DELAY));
    }
}

uint32_t round_to_int(double n)
{
    return n + 0.5;
}