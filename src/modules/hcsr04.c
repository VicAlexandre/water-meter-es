#include "hcsr04.h"
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY 2
#define DELAY 250

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

static const struct gpio_dt_spec trig = GPIO_DT_SPEC_GET(DT_NODELABEL(trig), gpios);
static const struct gpio_dt_spec echo = GPIO_DT_SPEC_GET(DT_NODELABEL(echo), gpios);

uint32_t distance_cm = -1; // last measured distance in cm

int hcsr04_init(void)
{
    int ret;

    if (!gpio_is_ready_dt(&trig) ||
        !gpio_is_ready_dt(&echo)) return -ENODEV;

    ret = gpio_pin_configure_dt(&trig, GPIO_OUTPUT);
    if (ret != 0) return ret;

    ret = gpio_pin_configure_dt(&echo, GPIO_INPUT);
    if (ret != 0) return ret;

    k_thread_create(&thread_data, hcsr04_stack_area,
                    K_THREAD_STACK_SIZEOF(hcsr04_stack_area),
                    calculate_distance,
                    NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    return 0;
}

int hcsr04_read_distance(struct hcsr04_data *data)
{
    if (distance_cm < 0) {
        return -ENODEV;
    }
    k_timepoint_t timepoint;
    timepoint.tick = k_uptime_ticks();

    data->distance = distance_cm;
    data->timestamp = timepoint;

    return 0;
}

void calculate_distance(void *p1, void *p2, void *p3)
{
    while (1) {
        // fire ultrasonic burst
        gpio_pin_set_dt(&trig, 1);
        k_usleep(10);
        gpio_pin_set_dt(&trig, 0);

        int ret = 0;

        // wait for HIGH
        do {
            ret = gpio_pin_get_dt(&echo);
            k_sleep(K_MSEC(10));
        } while (ret == 0);

        uint32_t start = k_cycle_get_32();

        // wait for LOW
        do {
            ret = gpio_pin_get_dt(&echo);
            k_sleep(K_MSEC(10));
        } while (ret == 1);

        // calculate distance
        uint32_t cycles = k_cycle_get_32() - start;
        double seconds = (double) cycles / sys_clock_hw_cycles_per_sec();

        double distance_m = (seconds * 343) / 2; // speed of sound: 343 m/s
        distance_cm = round_to_int(distance_m * 100);

        // apply delay
        k_sleep(K_MSEC(DELAY));
    }
}

uint32_t round_to_int(double n)
{
    return n + 0.5;
}
