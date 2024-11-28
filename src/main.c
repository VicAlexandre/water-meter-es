#include "modules/ble_lm.h"
#include "modules/hcsr04.h"
#include "modules/data_manager.h"
#include "modules/guards.h"

#include <stdio.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>

static void send_alarm(const struct shell *shell, size_t argc, char **argv);
static void send_critical_alarm(const struct shell *shell, size_t argc, char **argv);
static void get_man_data(const struct shell *shell, size_t argc, char **argv);
static void get_man_data_bulk(const struct shell *shell, size_t argc, char **argv);

SHELL_CMD_REGISTER(alarm, NULL, "Sends an alarm through BLE", send_alarm);
SHELL_CMD_REGISTER(critical_alarm, NULL, "Sends an critical alarm through BLE",
		   send_critical_alarm);
SHELL_CMD_REGISTER(man_get, NULL, "Gets last data polled by the manager", get_man_data);
SHELL_CMD_REGISTER(man_get_bulk, NULL, "Gets last n data polled by the manager", get_man_data_bulk);

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

static void handle_alarm(size_t argc, char **argv, int (*send)(struct hcsr04_data *))
{
	if (argc != 2) {
		printf("Usage: %s <distance>\n", argv[0]);
		return;
	}

	uint32_t distance = atoi(argv[1]);
	if (distance <= 0) {
		printf("The distance must be positive!\n");
		return;
	}

	k_timepoint_t timepoint;
	timepoint.tick = k_uptime_ticks();

	struct hcsr04_data data;
	data.distance = distance;
	data.timestamp = timepoint;

	int ret = send(&data);
	if (ret == 0) {
		printf("Alarm was sent\n");
	} else {
		printf("An error ocurred: %d\n", ret);
	}
}

static void send_alarm(const struct shell *shell, size_t argc, char **argv)
{
	handle_alarm(argc, argv, ble_send_alarm);
}

static void send_critical_alarm(const struct shell *shell, size_t argc, char **argv)
{
	handle_alarm(argc, argv, ble_send_critical_alarm);
}

static void get_man_data(const struct shell *shell, size_t argc, char **argv)
{
	struct hcsr04_data data;
	data_man_get(&data);
	printf("Distance: %d\nTimestamp: %lld\n", data.distance, data.timestamp.tick);
}

static void get_man_data_bulk(const struct shell *shell, size_t argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s <n>\n", argv[0]);
		return;
	}

	int n = atoi(argv[1]);
	if (n <= 0) {
		printf("n must be positive!\n");
		return;
	}

	struct hcsr04_data *data = (struct hcsr04_data *)k_malloc(sizeof(struct hcsr04_data) * n);

	int ret = data_man_get_bulk(&data, n);
	if (ret != 0) {
		printf("An error ocurred: %d\n", ret);
		return;
	}

	for (int i = 0; i < n; i++) {
		printf("[%d]\nDistance: %d\nTimestamp: %lld\n", i, data[i].distance,
		       data[i].timestamp.tick);
	}
	k_free(data);
}
