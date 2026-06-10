cmake_minimum_required(VERSION 3.20)

project(bsp C)

add_library(bsp STATIC
<<<<<<< HEAD
    src/bsp_gpio.c
    src/bsp_spi.c
    src/bsp_uart.c
    src/bsp_pwm.c
    src/bsp_adc.c
=======
        src/bsp_gpio.c
        src/bsp_spi.c
        src/bsp_adc.c
        src/bsp_pwm.c
        src/bsp_usart.c
        src/bsp_dma.c
>>>>>>> 96902d9086032e46ff5e189defcd61c3b3530f10
)

target_include_directories(bsp
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)