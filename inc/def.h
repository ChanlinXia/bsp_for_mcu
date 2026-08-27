//
// Created by Chanlin on 2026/5/15，位带操作定义
//

#ifndef BSP_DEF_H
#define BSP_DEF_H

#include "stm32f1xx.h"
#include <stdint.h>  // 引入标准数据类型定义

//**********************************************************************
// 以下为 STM32F4/F4 修改部分 (Cortex-M4)
//**********************************************************************

// 1. 位带操作宏定义 (保持不变，M4 同样适用)
// 外设位带区: 0X4000 0000 ~ 0X400F 0000, 映射到 0X4200 0000 ~ 0X43FF FFFF
#define BITBAND(addr,bitnum) ((addr&0xF0000000)+0x2000000+((addr&0xFFFFF)<<5)+(bitnum<<2))

// #define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

// 2. GPIO 基地址 (修改重点：从 APB2 改为 AHB1)
// F4 系列基地址参考 [citation:1][citation:6]
// #define GPIOA_BASE           0x40020000UL
// #define GPIOB_BASE           0x40020400UL
// #define GPIOC_BASE           0x40020800UL
// #define GPIOD_BASE           0x40020C00UL
// #define GPIOE_BASE           0x40021000UL
// #define GPIOF_BASE           0x40021400UL
// #define GPIOG_BASE           0x40021800UL

// 3. 寄存器地址映射 (修改重点：ODR/IDR 偏移量改变了)
// 在 F4 中，IDR 偏移为 0x10，ODR 偏移为 0x14 [citation:3][citation:7]
// 原 F1 代码是 +8 (IDR) 和 +12 (ODR)，这是错误的，虽然可能因重映射巧合能跑 [citation:10]
#define GPIOA_ODR_Addr    (GPIOA_BASE + 0x14) // F4 偏移量
#define GPIOB_ODR_Addr    (GPIOB_BASE + 0x14)
#define GPIOC_ODR_Addr    (GPIOC_BASE + 0x14)
#define GPIOD_ODR_Addr    (GPIOD_BASE + 0x14)
#define GPIOE_ODR_Addr    (GPIOE_BASE + 0x14)
#define GPIOF_ODR_Addr    (GPIOF_BASE + 0x14)
#define GPIOG_ODR_Addr    (GPIOG_BASE + 0x14)

#define GPIOA_IDR_Addr    (GPIOA_BASE + 0x10) // F4 偏移量
#define GPIOB_IDR_Addr    (GPIOB_BASE + 0x10)
#define GPIOC_IDR_Addr    (GPIOC_BASE + 0x10)
#define GPIOD_IDR_Addr    (GPIOD_BASE + 0x10)
#define GPIOE_IDR_Addr    (GPIOE_BASE + 0x10)
#define GPIOF_IDR_Addr    (GPIOF_BASE + 0x10)
#define GPIOG_IDR_Addr    (GPIOG_BASE + 0x10)

// 4. IO 口操作宏 (使用方法和原来完全一样)
// 确保 n 的值小于 16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr, n)  // 输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr, n)  // 输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr, n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr, n)

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr, n)
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr, n)

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr, n)
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr, n)

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr, n)
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr, n)

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr, n)
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr, n)

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr, n)
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr, n)

#endif // _DEF_H_
