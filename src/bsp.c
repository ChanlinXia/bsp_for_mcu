/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update   
*********************************************************************************************************/
#include "bsp.h"
#include <stdio.h>

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
    // ENUM_DEVICE_LED_START,
    // ENUM_DEVICE_ADC_START,
    // ENUM_DEVICE_SPI_START,
    // ENUM_DEVICE_PWM_START
    ENUM_DEVICE_MAX
    };

/*********************************************************************************************************
*                                              static Function
*********************************************************************************************************/
static void _BSP_Register(ENUM_DEVICE_ID dev_type,void* config) {
    switch (dev_type) { // use register fun for different device here
    case ENUM_DEVICE_GPIO_START:
        GPIO_DevRegister(config);
        break;
    // case ENUM_DEVICE_LED_START:
    //     GPIO_DevRegister(config);
    //     break;
    // case ENUM_DEVICE_ADC_START:
    //     break;
    // case ENUM_DEVICE_SPI_START:
    //     break;
    // case ENUM_DEVICE_PWM_START:
    //     break;
        default:
            break;
    }
}

static void _BSP_GetById(ENUM_DEVICE_ID dev_type,void* config) {
    switch (dev_type) { // use register fun for different device here
        case ENUM_DEVICE_GPIO_START:
            // GPIO_DevGet()
            break;
            // case ENUM_DEVICE_LED_START:
            //     GPIO_DevRegister(config);
            //     break;
            // case ENUM_DEVICE_ADC_START:
            //     break;
            // case ENUM_DEVICE_SPI_START:
            //     break;
            // case ENUM_DEVICE_PWM_START:
            //     break;
        default:
            break;
    }
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
    /*
    ENUM_DEVICE_GPIO_START =0,
ENUM_DEVICE_GPIO_EMS,
ENUM_DEVICE_GPIO_PWR_SW,
ENUM_DEVICE_GPIO_HB1_EN,
ENUM_DEVICE_GPIO_HB2_EN,
ENUM_DEVICE_GPIO_HB3_EN,
ENUM_DEVICE_GPIO_HB4_EN,

    ENUM_DEVICE_LED_START,
    ENUM_DEVICE_LED_POVER_G,
    ENUM_DEVICE_LED_POVER_R,
    ENUM_DEVICE_LED_STATUS,
    ENUM_DEVICE_LED_ERR,
    ENUM_DEVICE_LED_HEART,

*/
    void* register_list[ENUM_DEVICE_MAX]={
        // gpio
        &(dev_gpio_conf){GPIOB,GPIO_PIN_14},
        &(dev_gpio_conf){GPIOA,GPIO_PIN_1},
        &(dev_gpio_conf){GPIOC,GPIO_PIN_8},
        &(dev_gpio_conf){GPIOB,GPIO_PIN_1},
        &(dev_gpio_conf){GPIOC,GPIO_PIN_4},
        &(dev_gpio_conf){GPIOA,GPIO_PIN_5},

        // led
        &(dev_gpio_conf){GPIOC,GPIO_PIN_9},
        &(dev_gpio_conf){GPIOA,GPIO_PIN_8},
        &(dev_gpio_conf){GPIOA,GPIO_PIN_9},
        &(dev_gpio_conf){GPIOA,GPIO_PIN_9},
        &(dev_gpio_conf){GPIOB,GPIO_PIN_15},
        &(dev_gpio_conf){GPIOD,GPIO_PIN_2},

        // 

    };

    int i =0,j=0;
    ENUM_DEVICE_ID dev_type = ENUM_DEVICE_GPIO_START;
    for (i=0;i<ENUM_DEVICE_MAX;++i) {
        if (i == s_dev_satrt_list[j]) {
            dev_type = s_dev_satrt_list[j];
            ++j;
            continue;
        }
        _BSP_Register(dev_type,register_list[i]);
    }
}

/*********************************************************************************************************
*   get the virtual function of the device with the id
*
*   @param   id     the id of the device
*   @param   p_dev  the pointer of the device required
*   @return  void
*   @note id must be contained in the enum declaration in bsp_conf.h; p_dev will be the pointer of static device variable
*********************************************************************************************************/
void BSP_GetDevice(ENUM_DEVICE_ID id,bsp_device_t** p_dev) {
    int i=0;
    ENUM_DEVICE_ID dev_type = s_dev_satrt_list[i];

    while (1) {
        if (dev_type == ENUM_DEVICE_MAX) {
            _BSP_Debug("the id is valid");
            return;
        }

        if (id > dev_type) {
            i++;
        }
        else break;
        dev_type = s_dev_satrt_list[i];
    }

    dev_type = s_dev_satrt_list[i-1]; // 类型
    _BSP_GetById(dev_type,&p_dev);
}


