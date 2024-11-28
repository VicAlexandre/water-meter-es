#include <zephyr/ztest.h>

void test_main(void)
{
	ztest_test_suite(my_test_suite);
	ztest_run_test_suite(my_test_suite);
}

ZTEST_SUITE(my_test_suite, NULL, NULL, NULL, NULL, NULL);

ZTEST(my_test_suite, test_example)
{
	zassert_true(1 == 1, "Test passed");
}
