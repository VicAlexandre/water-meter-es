zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Registre os testes com Zephyr
target_sources(app PRIVATE test_main.c)
