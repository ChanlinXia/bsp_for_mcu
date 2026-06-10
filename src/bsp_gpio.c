/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update   
*********************************************************************************************************/
/*********************************************************************************************************
*                                              Header
*********************************************************************************************************/
#include "bsp_gpio.h"

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_GPIO_NUM

#ifdef  DEV_LED_NUM
#define GPIO_NUM  DEV_GPIO_NUM + DEV_LED_NUM
#else
#define GPIO_NUM  DEV_GPIO_NUM
#endif

# else
#define GPIO_NUM  1
#endif

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_gpio
{
    struct dev_gpio_vt* vt;

    uint32_t pin;
    GPIO_TypeDef* port;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void _set_up(struct dev_gpio_vt* self);
static void _set_down(struct dev_gpio_vt* self);
static uint8_t _read(struct dev_gpio_vt* self);
static void _toggle(struct dev_gpio_vt* self);

// static virtual function list
static struct dev_gpio s_dev_gpio_list[GPIO_NUM]={};
static struct dev_gpio_vt s_gpio_vt ={
    .set_up = _set_up,
    .set_down = _set_down,
    .read = _read,
    .toggle = _toggle,
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/
/*********************************************************************************************************
*   pull up the pin
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
static void _set_up(struct dev_gpio_vt* self) {
    struct dev_gpio* this = (struct dev_gpio*) self;
    HAL_GPIO_WritePin(this->port,this->pin, GPIO_PIN_SET);
}

/*********************************************************************************************************
*   pull down the pin
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
static void _set_down(struct dev_gpio_vt* self) {
    struct dev_gpio* this = (struct dev_gpio*) self;
    HAL_GPIO_WritePin(this->port,this->pin, GPIO_PIN_RESET);
}

/*********************************************************************************************************
*   read the pin
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
static uint8_t _read(struct dev_gpio_vt* self) {
    struct dev_gpio* this = (struct dev_gpio*) self;
    GPIO_PinState status=HAL_GPIO_ReadPin(this->port,this->pin);
    if (status == GPIO_PIN_SET) return 1;
    else return 0;
}

/*********************************************************************************************************
*   pull up the pin
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
static void _toggle(struct dev_gpio_vt* self) {
    struct dev_gpio* this = (struct dev_gpio*) self;
    HAL_GPIO_TogglePin(this->port,this->pin);
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   init the gpio dev with the conf
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
void GPIO_DevRegister(void* conf) {
    static uint8_t s_cnt=0;

    BSP_Assert(s_cnt < GPIO_NUM,"Fail to get the GPIO dev");


    struct dev_gpio* obj = &s_dev_gpio_list[s_cnt++];
    dev_gpio_conf* gpio_conf = (dev_gpio_conf*)conf;

    obj->vt = &s_gpio_vt;
    obj->pin = gpio_conf->pin;
    obj->port = gpio_conf->port;
}

/*********************************************************************************************************
*   get the gpio dev
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
void GPIO_DevGet(struct dev_gpio_vt** obj,uint8_t ind) {
    BSP_Assert(ind < GPIO_NUM,"Fail to get the GPIO dev");

    *obj = (struct dev_gpio_vt*)&s_dev_gpio_list[ind]; // 赋值
}

