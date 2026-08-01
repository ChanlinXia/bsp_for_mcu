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

/*********************************************************************************************************
*                                              Header File
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Macro
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Structure
*********************************************************************************************************/
static uint8_t s_dev_satrt_list[] = {
    ENUM_DEVICE_GPIO_START,
    ENUM_DEVICE_ADC_START,
    ENUM_DEVICE_SPI_START,
    ENUM_DEVICE_PWM_START,
    ENUM_DEVICE_PWM_PAIR_START,
    ENUM_CHIP_TPC1_S4_START,
    ENUM_DEVICE_MAX
    };

/*********************************************************************************************************
*                                              static Function
*********************************************************************************************************/
static void _BSP_Register(ENUM_BSP_ID dev_type,void* config) {
    switch (dev_type) { // use register fun for different device here
        case ENUM_DEVICE_GPIO_START:
            GPIO_DevRegister(config);
            break;
        // case ENUM_DEVICE_ADC_START:
        //     break;

        case ENUM_DEVICE_SPI_START:
            SPI_DevRegister(config);
            break;

        case ENUM_DEVICE_PWM_START:
            PWM_DevRegister(config);
            break;

        case ENUM_DEVICE_PWM_PAIR_START:
            PWMPair_DevRegister(config);
            break;

        case ENUM_CHIP_TPC1_S4_START:
            TPC1S4_DevRegister(config);
            break;

        default:
            break;
    }
}

static void* _BSP_GetById(ENUM_BSP_ID dev_type,uint8_t ind) {
    void* p_dev = 0;
    switch (dev_type) { // use register fun for different device here
        case ENUM_DEVICE_GPIO_START:
            GPIO_DevGet((struct dev_gpio**)&p_dev,ind);
            break;

        // case ENUM_DEVICE_ADC_START:
        //     break;

        case ENUM_DEVICE_SPI_START:
            SPI_DevGet((struct dev_spi**)&p_dev,ind);
            break;

        case ENUM_DEVICE_PWM_START:
            PWM_DevGet((struct dev_pwm**)&p_dev,ind);
            break;

        case ENUM_DEVICE_PWM_PAIR_START:
            PWMPair_DevGet((struct dev_pwm_pair**)&p_dev,ind);
            break;

        case ENUM_CHIP_TPC1_S4_START:
            TPC1S4_DevGet((struct tpc1s4tr_t**)&p_dev,ind);
            break;

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

    void* register_list[]={
        // gpio
        &(dev_gpio_conf){GPIOB,GPIO_PIN_14}, //ENUM_DEVICE_GPIO_EMS
        &(dev_gpio_conf){GPIOA,GPIO_PIN_1},  //ENUM_DEVICE_GPIO_PWR_SW
        &(dev_gpio_conf){GPIOC,GPIO_PIN_8},  //ENUM_DEVICE_GPIO_HB1_EN
        &(dev_gpio_conf){GPIOB,GPIO_PIN_1},  //ENUM_DEVICE_GPIO_HB2_EN
        &(dev_gpio_conf){GPIOC,GPIO_PIN_4},  //ENUM_DEVICE_GPIO_HB3_EN
        &(dev_gpio_conf){GPIOA,GPIO_PIN_5},  //ENUM_DEVICE_GPIO_HB4_EN
        &(dev_gpio_conf){GPIOA,GPIO_PIN_5},  //ENUM_DEVICE_GPIO_DAC_CHIP_EN

        // led
        &(dev_gpio_conf){GPIOC,GPIO_PIN_9,GPIO_PIN_RESET},  //ENUM_DEVICE_LED_POWER_G
        &(dev_gpio_conf){GPIOA,GPIO_PIN_8,GPIO_PIN_RESET},  //ENUM_DEVICE_LED_POWER_R
        &(dev_gpio_conf){GPIOA,GPIO_PIN_9,GPIO_PIN_RESET},  //ENUM_DEVICE_LED_STATUS
        &(dev_gpio_conf){GPIOA,GPIO_PIN_9,GPIO_PIN_RESET},  //ENUM_DEVICE_LED_ERR
        &(dev_gpio_conf){GPIOB,GPIO_PIN_15,GPIO_PIN_RESET}, //ENUM_DEVICE_LED_HEART
        // &(dev_gpio_conf){GPIOD,GPIO_PIN_2},

        // ADC
        &(dev_adc_conf){&hadc1},

        // SPI
        &(dev_spi_conf){ // DAC Chip
            .hspi = &hspi1,
            .cs_pin = BSP_GetDevice(ENUM_DEVICE_GPIO_DAC_CHIP_EN),
            .use_soft_cs = 0
        },

        // &(dev_spi_conf){
        //     .hspi = &hspi3,
        //     .cs_pin = BSP_GetDevice(ENUM_DEVICE_GPIO_DAC_CHIP_EN),
        //     .use_soft_cs = 0
        // },

        // PWM
        // ==================== 桥1 (TIM8) ====================
        &(dev_pwm_basic_config){&htim8, TIM_CHANNEL_1, M_Hz(BSP_PCLK2*2)}, //  ENUM_DEVICE_PWM_BRIDGE_H_1  (PC6)
        &(dev_pwm_basic_config){&htim8, TIM_CHANNEL_2,M_Hz (BSP_PCLK2*2)}, //  ENUM_DEVICE_PWM_BRIDGE_L_1  (PC7)

        // ==================== 桥2 (TIM2) ====================
        &(dev_pwm_basic_config){&htim2, TIM_CHANNEL_3,M_Hz (BSP_PCLK1*2)}, //  ENUM_DEVICE_PWM_BRIDGE_H_2  (PB10)
        &(dev_pwm_basic_config){&htim2, TIM_CHANNEL_4,M_Hz (BSP_PCLK1*2)}, //  ENUM_DEVICE_PWM_BRIDGE_L_2  (PB11)

        // ==================== 桥3 (TIM3) ====================
        &(dev_pwm_basic_config){&htim3, TIM_CHANNEL_1,M_Hz (BSP_PCLK1*2)}, //  ENUM_DEVICE_PWM_BRIDGE_H_3  (PA6)
        &(dev_pwm_basic_config){&htim3, TIM_CHANNEL_2,M_Hz (BSP_PCLK1*2)}, //  ENUM_DEVICE_PWM_BRIDGE_L_3  (PA7)

        // ==================== 桥4 (TIM9) ====================
        &(dev_pwm_basic_config){&htim9, TIM_CHANNEL_1,M_Hz (BSP_PCLK2*2)}, //  ENUM_DEVICE_PWM_BRIDGE_H_4  (PA2)
        &(dev_pwm_basic_config){&htim9, TIM_CHANNEL_2, M_Hz(BSP_PCLK2*2)}, //  ENUM_DEVICE_PWM_BRIDGE_L_4  (PA3)

        // PWM_PAIR
        &(dev_pwm_pair_conf){ // pwm_pair 1 (for birdge)
            .pwm_h = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_H_1),
            .pwm_l = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_L_1),
            .dma_enabled = 0,
            .dead_time_tick = 20,
        },

        &(dev_pwm_pair_conf){ // pwm_pair 2 (for birdge)
            .pwm_h = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_H_2),
            .pwm_l = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_L_2),
            .dma_enabled = 1,
            .dead_time_tick = 20,
        },

        &(dev_pwm_pair_conf){ // pwm_pair 3 (for birdge)
            .pwm_h = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_H_3),
            .pwm_l = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_L_3),
            .dma_enabled = 1,
            .dead_time_tick = 20,
        },

        &(dev_pwm_pair_conf){ // pwm_pair 2 (for birdge)
            .pwm_h = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_H_4),
            .pwm_l = BSP_GetDevice(ENUM_DEVICE_PWM_BRIDGE_L_4),
            .dma_enabled = 1,
            .dead_time_tick = 20,
        },

        /*
         * Chip
         */
        // CHIP_TPC1_S4
        &(chip_tpc1s4tr_config_t){
            .ref = 2.048,
            .ptr_dev_spi = BSP_GetDevice(ENUM_DEVICE_SPI1_DAC_CHIP),
            .chip_type=1	// 1:116 0:112
        }
    };

    int i =0,j=0;
    ENUM_BSP_ID dev_type = 0;

    do {
        // printf("i val :%d\r\n",i);
        if (i == s_dev_satrt_list[j]) {
            dev_type = s_dev_satrt_list[j];
            ++i;
            ++j;
            continue;
        }
        _BSP_Register(dev_type,register_list[i-j]);
        i++;
    }while (ENUM_DEVICE_MAX != dev_type);

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


