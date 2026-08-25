/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/4.
*   @version  1.0
*   @update   
*********************************************************************************************************/

#ifndef BSP_CONF_H
#define BSP_CONF_H

/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

/*********************************************************************************************************
*                                              Macro
*********************************************************************************************************/
#define BSP_ENABLE_ASSERT   1

#if BSP_ENABLE_ASSERT == 1
#define BSP_Assert(x,info,ind) do{ if(!(x)){ printf("BSP_Assert at [%s:%d]: %s, device id :%d\r\n", __FILE__, __LINE__,info,ind); while(1){}}}while(0)

#else
#define BSP_Debug(x,info,rval) do{ if(!(x)){ printf("BSP_Assert at [%s:%d]: %s\n", __FILE__, __LINE__,info); }  return rval}while(0)
#define BSP_Assert(x,info,rval) BSP_Debug(x,info,rval)

#endif

// 系统时钟
#define BSP_SYSCLK 168
#define BSP_PCLK1 42
#define BSP_PCLK2 84
#define M_Hz(x) ((x) * 1000000)


// device num
#define DEV_GPIO_NUM 8
#define DEV_LED_NUM 5
#define DEV_SPI_NUM 2
#define DEV_ADC_NUM 1
// #define DEV_UART_NUM 1
#define DEV_PWM_NUM 8
// #define DEV_PWM_PAIR_NUM 4


#define CHIP_TPC1_S4_NUM 1
// #define CHIP_TPC112S4_NUM 0

/*********************************************************************************************************
*                                              Enum
*********************************************************************************************************/
// 注册表
typedef enum {
    /*
     * GPIO
     */
#ifdef DEV_GPIO_NUM

    ENUM_DEVICE_GPIO_START =0,
    ENUM_DEVICE_GPIO_EMS,
    ENUM_DEVICE_GPIO_PWR_SW,
    ENUM_DEVICE_GPIO_HB1_EN,
    ENUM_DEVICE_GPIO_HB2_EN,
    ENUM_DEVICE_GPIO_HB3_EN,
    ENUM_DEVICE_GPIO_HB4_EN,
	ENUM_DEVICE_GPIO_DAC_CHIP_CS,
	ENUM_DEVICE_GPIO_DAC_CHIP_LOAD,
#endif

	/*
	 * LED
	 */
#ifdef DEV_LED_NUM
    // ENUM_DEVICE_LED_START,
    ENUM_DEVICE_LED_POWER_G,
    ENUM_DEVICE_LED_POWER_R,
    ENUM_DEVICE_LED_STATUS,
    ENUM_DEVICE_LED_ERR,
    ENUM_DEVICE_LED_HEART,

    // ENUM_DEVICE_LED_TEST1,
    // ENUM_DEVICE_LED_TEST2,
#endif

	/*
	 * ADC
	 */
#ifdef DEV_ADC_NUM
    ENUM_DEVICE_ADC_START,
    ENUM_DEVICE_BATTERY_ADC,    // 电池ADC
#endif

	/*
	 * SPI
	 */
#ifdef DEV_SPI_NUM
    ENUM_DEVICE_SPI_START,
    ENUM_DEVICE_SPI1_DAC_CHIP,
    ENUM_DEVICE_SPI3_RF,
#endif

	/*
	* UART
	*/
#ifdef DEV_UART_NUM

    // UART

#endif

    /*
     * PWM
     */
#ifdef DEV_PWM_NUM
    ENUM_DEVICE_PWM_START,
    ENUM_DEVICE_PWM_BRIDGE_H_1,
	ENUM_DEVICE_PWM_BRIDGE_L_1,
	ENUM_DEVICE_PWM_BRIDGE_H_2,
	ENUM_DEVICE_PWM_BRIDGE_L_2,
	ENUM_DEVICE_PWM_BRIDGE_H_3,
	ENUM_DEVICE_PWM_BRIDGE_L_3,
	ENUM_DEVICE_PWM_BRIDGE_H_4,
	ENUM_DEVICE_PWM_BRIDGE_L_4,
#endif

	/*
	* PWM_PAIR
	*/
#if defined( DEV_PWM_PAIR_NUM )
	ENUM_DEVICE_PWM_PAIR_START,
	ENUM_DEVICE_PWM_PAIR_BRIDGE_1,
	ENUM_DEVICE_PWM_PAIR_BRIDGE_2,
	ENUM_DEVICE_PWM_PAIR_BRIDGE_3,
	ENUM_DEVICE_PWM_PAIR_BRIDGE_4,

#endif

	/*****************************
	 *		CHIP PART
	 *****************************/
#ifdef CHIP_TPC1_S4_NUM
	ENUM_CHIP_TPC1_S4_START,
	ENUM_CHIP_TPC116S4_REF_VOL_DAC,
#endif


    ENUM_DEVICE_MAX
}ENUM_BSP_ID;

/*********************************************************************************************************
*                                              Structure
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API List
*********************************************************************************************************/
// void BSP_Debug();

#endif //FACIAL_STIMULATION_DEVICE_BSP_CONF_H
