cmake_minimum_required(VERSION 3.20)

project(bsp C)

add_library(bsp STATIC
    src/bsp_gpio.c
    src/bsp_spi.c
)

target_include_directories(bsp
PUBLIC // 库封装关键
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)