#include "modules/ble_lm.h"
#include "modules/hcsr04.h"
#include "modules/data_manager.h"
#include "modules/guards.h"

#include <stdio.h>

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
