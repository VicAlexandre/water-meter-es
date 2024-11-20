#include "modules/ble_lm.h"
#include "modules/hcsr04.h"
#include "modules/data_manager.h"
#include "modules/guards.h"

#include <stdio.h>

static uint32_t distance(const struct shell *shell, size_t argc, char **argv)
{
	uint32_t distance;
	hcsr04_read_distance(&distance);
	return distance;
}

static int alarm(const struct shell *shell, size_t argc, char **argv)
{
	int data;
	ble_send_alarm(&data);
	return data;
}

static int critical_alarm(const struct shell *shell, size_t argc, char **argv)
{
	int data;
	ble_send_critical_alarm(&data);
	return data;
}

static int data_get(const struct shell *shell, size_t argc, char **argv)
{
	int data;
	data_man_get(&data);
	return data;
}

static int data_get_bulk(const struct shell *shell, size_t argc, char **argv)
{
	int data;
	data_man_get_bulk(&data);
	return data;
}

SHELL_CMD_REGISTER(distance, NULL, "Return the measured distance by the sensor", distance);
SHELL_CMD_REGISTER(alarm, NULL, "Sends an alarm through BLE", alarm);
SHELL_CMD_REGISTER(critical_alarm, NULL, "Sends a alarm through BLE", critical_alarm);
SHELL_CMD_REGISTER(get, NULL, "Gets last data polled by the manager", data_get);
SHELL_CMD_REGISTER(get_bulk, NULL, "Gets last n data polled by the manager", data_get_bulk);

int main(void)
{
	struct data_man_conf conf = {
		.poll_interval = K_MSEC(CONFIG_POLLING_DELAY_MS),
		.alarm_threshold = 10,
		.critical_alarm_threshold = 5,
	};
	LM_CHECK_ERROR(ble_init());
	LM_CHECK_ERROR(data_man_init(10, &conf));
	LM_CHECK_ERROR(hcsr04_init());



	return 0;
}
