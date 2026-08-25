/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update   
*********************************************************************************************************/
#include "bsp.h"
#include <stdio.h>

#include "bsp_pwm_pair.h"
#include "spi.h"
#include "stm32f4xx_hal_gpio.h"

/*********************************************************************************************************
*                                              Header File
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Macro
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Structure
*********************************************************************************************************/
// ========== 添加宏保护，与第一段一致 ==========
static uint8_t s_dev_satrt_list[] = {
#if defined(DEV_GPIO_NUM)
    ENUM_DEVICE_GPIO_START,
#endif
#if defined(DEV_ADC_NUM)
    ENUM_DEVICE_ADC_START,
#endif
#if defined(DEV_SPI_NUM)
    ENUM_DEVICE_SPI_START,
#endif
#if defined(DEV_PWM_NUM)
    ENUM_DEVICE_PWM_START,
#endif
// #if defined(DEV_PWM_PAIR_NUM)   // 原注释掉了，保留注释
//     ENUM_DEVICE_PWM_PAIR_START,
// #endif
#if defined(CHIP_TPC1_S4_NUM)
    ENUM_CHIP_TPC1_S4_START,
#endif
    ENUM_DEVICE_MAX
    };

/*********************************************************************************************************
*                                              static Function
*********************************************************************************************************/
static void _BSP_Register(ENUM_BSP_ID dev_type,void* config) {
    switch (dev_type) { // use register fun for different device here
#if defined(DEV_GPIO_NUM)
        case ENUM_DEVICE_GPIO_START:
            GPIO_DevRegister(config);
            break;
#endif
        // case ENUM_DEVICE_ADC_START:
        //     break;

#if defined(DEV_SPI_NUM)
        case ENUM_DEVICE_SPI_START:
            SPI_DevRegister(config);
            break;
#endif

#if defined(DEV_PWM_NUM)
        case ENUM_DEVICE_PWM_START:
            PWM_DevRegister(config);
            break;
#endif

        // #if defined(DEV_PWM_PAIR_NUM)
        // case ENUM_DEVICE_PWM_PAIR_START:
        //     PWMPair_DevRegister(config);
        //     break;
        // #endif

#if defined(CHIP_TPC1_S4_NUM)
        case ENUM_CHIP_TPC1_S4_START:
            TPC1S4_DevRegister(config);
            break;
#endif

        default:
            break;
    }
}

static void* _BSP_GetById(ENUM_BSP_ID dev_type,uint8_t ind) {
    void* p_dev = 0;
    switch (dev_type) { // use register fun for different device here
#if defined(DEV_GPIO_NUM)
        case ENUM_DEVICE_GPIO_START:
            GPIO_DevGet((struct dev_gpio**)&p_dev,ind);
            break;
#endif

        // case ENUM_DEVICE_ADC_START:
        //     break;

#if defined(DEV_SPI_NUM)
        case ENUM_DEVICE_SPI_START:
            SPI_DevGet((struct dev_spi**)&p_dev,ind);
            break;
#endif

#if defined(DEV_PWM_NUM)
        case ENUM_DEVICE_PWM_START:
            PWM_DevGet((struct dev_pwm**)&p_dev,ind);
            break;
#endif

        // #if defined(DEV_PWM_PAIR_NUM)
        // case ENUM_DEVICE_PWM_PAIR_START:
        //     PWMPair_DevGet((struct dev_pwm_pair**)&p_dev,ind);
        //     break;
        // #endif

#if defined(CHIP_TPC1_S4_NUM)
        case ENUM_CHIP_TPC1_S4_START:
            TPC1S4_DevGet((struct tpc1s4tr_t**)&p_dev,ind);
            break;
#endif

        default:
            break;
    }

    if (p_dev == 0) return NULL;
    else  return p_dev;
}

static void _BSP_Debug(const char* debug_info) {
    printf("%s",debug_info);
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   register all kinds of devices
*
*   @param   void
*   @return  void
*   @note   it will register the devices to be used by the order in bsp_conf.h
*********************************************************************************************************/
void BSP_Init(void) {

    // ========== GPIO (包括 LED) ==========
#if defined(DEV_GPIO_NUM) && (DEV_GPIO_NUM > 0)
    // 定义 GPIO 配置数组（来自原 register_list）
    dev_gpio_conf gpio_configs[] = {
    // ========== GPIO 部分（对应 ENUM_DEVICE_GPIO_*） ==========
    {GPIOB, GPIO_PIN_14},                        


    // ========== LED 部分（对应 ENUM_DEVICE_LED_*） ==========
    {GPIOC, GPIO_PIN_9, GPIO_PIN_RESET},            // ENUM_DEVICE_LED_POWER_G      (PC9: 电源绿)
    };
    for (uint32_t i = 0; i < sizeof(gpio_configs)/sizeof(dev_gpio_conf); i++) {
        GPIO_DevRegister((void*)&gpio_configs[i]);
    }
#endif // GPIO

    // ========== ADC ==========
#if defined(DEV_ADC_NUM) && (DEV_ADC_NUM > 0)
    dev_adc_conf adc_configs[] = {
        {&hadc1},
    };
    for (uint32_t i = 0; i < sizeof(adc_configs)/sizeof(dev_adc_conf); i++) {
        // ADC_DevRegister((void*)&adc_configs[i]);  // 若需要可取消注释
    }
#endif

    // ========== SPI ==========
#if defined(DEV_SPI_NUM) && (DEV_SPI_NUM > 0)
    dev_spi_conf spi_configs[] = {
        { // DAC Chip
            .hspi = &hspi1,
            .cs_pin = BSP_GetDevice(ENUM_DEVICE_GPIO_DAC_CHIP_CS),
            .use_soft_cs = 0
        },
        // 若还有其他 SPI 可在此添加
    };
    for (uint32_t i = 0; i < sizeof(spi_configs)/sizeof(dev_spi_conf); i++) {
        SPI_DevRegister((void*)&spi_configs[i]);
    }
#endif

    // ========== PWM ==========
#if defined(DEV_PWM_NUM) && (DEV_PWM_NUM > 0)
    dev_pwm_basic_config pwm_configs[] = {

    };
    for (uint32_t i = 0; i < sizeof(pwm_configs)/sizeof(dev_pwm_basic_config); i++) {
        PWM_DevRegister((void*)&pwm_configs[i]);
    }
#endif

    // ========== PWM_PAIR ==========
#if defined(DEV_PWM_PAIR_NUM) && (DEV_PWM_PAIR_NUM > 0)
    dev_pwm_pair_conf pair_configs[] = {
        { // pwm_pair 1
            .pwm_h = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_H_1),
            .pwm_l = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_L_1),
            .dma_enabled = 0,
            .dead_time_tick = 20,
        },
        { // pwm_pair 2
            .pwm_h = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_H_2),
            .pwm_l = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_L_2),
            .dma_enabled = 1,
            .dead_time_tick = 20,
        },
        { // pwm_pair 3
            .pwm_h = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_H_3),
            .pwm_l = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_L_3),
            .dma_enabled = 1,
            .dead_time_tick = 20,
        },
        { // pwm_pair 4
            .pwm_h = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_H_4),
            .pwm_l = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_L_4),
            .dma_enabled = 1,
            .dead_time_tick = 20,
        },
    };
    for (uint32_t i = 0; i < sizeof(pair_configs)/sizeof(pair_configs[0]); i++) {
        PWMPair_DevRegister((void*)&pair_configs[i]);
    }
#endif

    // ========== Chip TPC1_S4 ==========
#if defined(CHIP_TPC1_S4_NUM) && (CHIP_TPC1_S4_NUM > 0)
    chip_tpc1s4tr_config_t chip_tpc1_configs[] = {
        {
            .ref = 2.048,
            .ptr_dev_spi = BSP_GetDevice(ENUM_DEVICE_SPI1_DAC_CHIP),
            .chip_type = 1,    // 1:116 0:112
            .load_pin = BSP_GetDevice(ENUM_DEVICE_GPIO_DAC_CHIP_LOAD),
        }
    };
    for (uint32_t i = 0; i < sizeof(chip_tpc1_configs)/sizeof(chip_tpc1s4tr_config_t); i++) {
        TPC1S4_DevRegister((void*)&chip_tpc1_configs[i]);
    }
#endif

    // 延时初始化（总是需要）
    delay_init(BSP_SYSCLK);
}

/*********************************************************************************************************
*   get the virtual function of the device with the id
*
*   @param   id     the id of the device
*   @return  void
*   @note id must be contained in the enum declaration in bsp_conf.h; p_dev will be the pointer of static device variable
*********************************************************************************************************/
void* BSP_GetDevice(ENUM_BSP_ID id) {
    int i=0;
    ENUM_BSP_ID dev_type = s_dev_satrt_list[i];

    while (1) {
        if (id > dev_type) {
            i++;
        }
        else break;

        if (dev_type >= ENUM_DEVICE_MAX) {
            _BSP_Debug("the id is valid");
            return NULL;
        }

        dev_type = s_dev_satrt_list[i];
    }

    dev_type = s_dev_satrt_list[i-1]; // 类型
    uint8_t ind = id-dev_type-1;
    printf("the device ind of devs:%d\r\n",id-dev_type-1);
    return _BSP_GetById(dev_type,id-dev_type-1);
}