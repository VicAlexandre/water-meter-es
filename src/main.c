#include "modules/ble_lm.h"
#include "modules/hcsr04.h"
#include "modules/data_manager.h"
#include "modules/guards.h"

#include <stdio.h>

static void distance(const struct shell *shell, size_t argc, char **argv)
{
	uint32_t distance;
	hcsr04_read_distance(&distance);
	printf("Current distance: %d\n", distance);
}

static void alarm(const struct shell *shell, size_t argc, char **argv)
{
	if(argc != 2) {
		printf("Usage: %s <distance>\n", argv[0]);
		return;
	}

	int distance = atoi(argv[1]);

	if(distance <= 0) {
		printf("Error: Please provide a positive value for the length in centimeters.\n");
		return;
	}

	struct hcsr04 data = {distance, k_cycle_get_32()};
	int ret = ble_send_alarm();
	if(ret == 0) {
		printf("Alarm sent\n");
	}
	else {
		printf("Error: %d\n", ret);
	}
}

static int critical_alarm(const struct shell *shell, size_t argc, char **argv)
{
	if(argc != 2) {
		printf("Usage: %s <distance>\n", argv[0]);
		return;
	}

	int distance = atoi(argv[1]);

	if(distance <= 0) {
		printf("Error: Please provide a positive value for the length in centimeters.\n");
		return;
	}

	struct hcsr04 data = {distance, k_cycle_get_32()};
	int ret = ble_send_alarm();
	if(ret == 0) {
		printf("Critical alarm sent\n");
	}
	else {
		printf("Error: %d\n", ret);
	}
}

static int data_get(const struct shell *shell, size_t argc, char **argv)
{
	struct hcsr04_data data;
	data_man_get(&data);
	printf("Distance: %d, Timestamp: %d\n", data.distance, data.timestamp);
}

static int data_get_bulk(const struct shell *shell, size_t argc, char **argv)
{
	if(argc != 2) {
		printf("Usage: %s <n>\n", argv[0]);
		return;
	}

	int n = atoi(argv[1]);

	if(n <= 0) {
		printf("Error: please provide a value that is not null\n");
		return;
	}
	
	struct hcsr04_data data;
	data_man_get_bulk(&data, n);
	printf("Distance: %d, Timestamp: %d\n", data.distance, data.timestamp);
}

SHELL_CMD_REGISTER(distance, NULL, "Return the measured distance by the sensor", distance);
SHELL_CMD_REGISTER(alarm, NULL, "Sends an alarm through BLE", alarm);
SHELL_CMD_REGISTER(critical_alarm, NULL, "Sends an alarm through BLE", critical_alarm);
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
