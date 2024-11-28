#include <zephyr/ztest.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "hcsr04.h"

static int gpio_pin_set_dt_mock(const struct gpio_dt_spec *spec, int value)
{
    ARG_UNUSED(spec);
    ARG_UNUSED(value);
    return 0; // Simula o sucesso de definir o pino
}

static int gpio_pin_get_dt_mock(const struct gpio_dt_spec *spec)
{
    ARG_UNUSED(spec);
    return 1; // Simula que o pino está sempre HIGH
}

static int gpio_pin_configure_dt_mock(const struct gpio_dt_spec *spec, gpio_flags_t flags)
{
    ARG_UNUSED(spec);
    ARG_UNUSED(flags);
    return 0; // Simula sucesso na configuração do pino
}

// Substitui as funções GPIO reais pelos mocks
#define gpio_pin_set_dt gpio_pin_set_dt_mock
#define gpio_pin_get_dt gpio_pin_get_dt_mock
#define gpio_pin_configure_dt gpio_pin_configure_dt_mock

ZTEST_SUITE(hcsr04_test_suite, NULL, NULL, NULL, NULL, NULL);

ZTEST(hcsr04_test_suite, test_hcsr04_init)
{
    int ret = hcsr04_init();

    // Verifica se a inicialização foi bem-sucedida (retorno 0)
    zassert_equal(ret, 0, "Failed to initialize HC-SR04 sensor");
}

ZTEST(hcsr04_test_suite, test_hcsr04_read_distance)
{
    struct hcsr04_data data;

    // Simula a leitura da distância
    int ret = hcsr04_read_distance(&data);

    // Verifica se a leitura foi bem-sucedida
    zassert_equal(ret, 0, "Failed to read distance");

    // Verifica se a distância foi preenchida (não deve ser -1)
    zassert_not_equal(data.distance, -1, "Invalid distance value");
}