#include "modules/ble_lm.h"
#include "modules/hcsr04.h"
#include "modules/data_manager.h"
#include "modules/guards.h"

#include <stdio.h>

int main(void)
{
	LM_CHECK_ERROR(ble_init());
	LM_CHECK_ERROR(hcsr04_init());

	printf("Hello World!\n");

	return 0;
}
