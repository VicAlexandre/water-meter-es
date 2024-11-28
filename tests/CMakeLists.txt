cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(main_testing)

target_sources(app PRIVATE src/test_main.c)
target_sources(app PRIVATE src/modules/test_hcsr04.c)
zephyr_include_directories(../src/modules)