cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(bsp C)

add_library(bsp STATIC
    src/bsp.c
    src/bsp_gpio.c
    src/bsp_spi.c
    src/bsp_uart.c
    src/bsp_pwm.c
    src/bsp_adc.c
    src/bsp_delay.c
    src/bsp_pwm_pair.c

    # chip
    chip/TPC1_S4_TR/TPC1_S4_TR.c
    chip/CC2500RGPR/CC2500RGPR.c
)

target_include_directories(bsp
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/chip/CC2500RGPR
    ${CMAKE_CURRENT_SOURCE_DIR}/chip/TPC1_S4_TR
)

target_link_libraries(bsp PUBLIC
    stm32cubemx
)