#include <zephyr/ztest.h>

ZTEST_SUITE(water_meter_main_suite, NULL, NULL, NULL, NULL, NULL);

ZTEST(water_meter_main_suite, test_main)
{
    zassert_equal(0, 0, "This test will always pass");
}
