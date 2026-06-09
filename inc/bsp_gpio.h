/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/4.
*   @version  1.0
*   @update   
*********************************************************************************************************/


#ifndef _BSP_CONF_H
#define _BSP_CONF_H

/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include <stdio.h>
/*********************************************************************************************************
*                                              Macro
*********************************************************************************************************/
#define Assert(x) do{ if(!x) printf("Assert at %s:%d\n", __FILE__, __LINE__); while(1){}}while(0)


#define DEV_GPIO_NUM 6
#define DEV_LED_NUM 5
#define DEV_SPI_NUM 2
#define DEV_ADC_NUM 1
// #define DEV_UART_NUM 1
#define DEV_PWM_NUM 8

/*********************************************************************************************************
*                                              Enum
*********************************************************************************************************/
// 注册表
typedef enum {
    // GPIO
#ifdef DEV_GPIO_NUM

    ENUM_DEVICE_GPIO_START =0,
    ENUM_DEVICE_GPIO_EMS,
    ENUM_DEVICE_GPIO_PWR_SW,
    ENUM_DEVICE_GPIO_HB1_EN,
    ENUM_DEVICE_GPIO_HB2_EN,
    ENUM_DEVICE_GPIO_HB3_EN,
    ENUM_DEVICE_GPIO_HB4_EN,
#endif

    // LED
#ifdef DEV_LED_NUM
    ENUM_DEVICE_LED_START,
    ENUM_DEVICE_LED_POVER_G,
    ENUM_DEVICE_LED_POVER_R,
    ENUM_DEVICE_LED_STATUS,
    ENUM_DEVICE_LED_ERR,
    ENUM_DEVICE_LED_HEART,
#endif

    // ADC
#ifdef DEV_ADC_NUM
    ENUM_DEVICE_ADC_START,
    ENUM_DEVICE_BATTERY_ADC,    // 电池ADC
#endif

    // SPI
#ifdef DEV_SPI_NUM
    ENUM_DEVICE_SPI_START,
    ENUM_DEVICE_SPI1_DAC_CHIP,
    ENUM_DEVICE_SPI3_BT,
#endif

#ifdef DEV_UART_NUM

    // UART

#endif


    // PWM
#ifdef DEV_PWM_NUM
    ENUM_DEVICE_PWM_START,
    ENUM_DEVICE_TIM2_C3_PWM,
    ENUM_DEVICE_TIM2_C4_PWM,
    ENUM_DEVICE_TIM3_C1_PWM,
    ENUM_DEVICE_TIM2_C2_PWM,
    ENUM_DEVICE_TIM9_C1_PWM,
    ENUM_DEVICE_TIM9_C2_PWM,
    ENUM_DEVICE_TIM8_C1_PWM,
    ENUM_DEVICE_TIM8_C2_PWM,
#endif
    ENUM_DEVICE_MAX
}ENUM_DEVICE_ID;

/*********************************************************************************************************
*                                              Structure
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API List
*********************************************************************************************************/
// void BSP_Debug();

#endif //FACIAL_STIMULATION_DEVICE_BSP_CONF_H
