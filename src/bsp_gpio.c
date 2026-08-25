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
#include <stdio.h>

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
struct dev_gpio_impl
{
    struct dev_gpio dev;

    uint8_t dev_id;
    uint32_t pin;
    GPIO_TypeDef* port;

    uint8_t led_active_level;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void set_up(struct dev_gpio* self);
static void set_down(struct dev_gpio* self);
static uint8_t read(struct dev_gpio* self);
static void toggle(struct dev_gpio* self);
static void write(struct dev_gpio* self,uint8_t on_off);

static void led_on(struct dev_gpio* self);
static void led_off(struct dev_gpio* self);
static uint8_t is_led_on(struct dev_gpio* self);


// static virtual function list
static struct dev_gpio_impl s_dev_gpio_list[GPIO_NUM]={};
static struct dev_gpio_vt s_gpio_vt ={
    .set_up = set_up,
    .set_down = set_down,
    .write = write,
    .read = read,
    .toggle = toggle,

    .led_on =led_on,
    .led_off =led_off,
    .is_led_on = is_led_on,
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
static void set_up(struct dev_gpio* self) {
    // printf("port:%d,pin:%d\r\n",this->port,this->pin);

    // printf("set up the gpio\r\n");
    struct dev_gpio_impl* this = (struct dev_gpio_impl*) self;
    // printf("port:%d,pin:%d\r\n",this->port,this->pin);
    HAL_GPIO_WritePin(this->port,this->pin, GPIO_PIN_SET);
}

/*********************************************************************************************************
*   pull down the pin
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
static void set_down(struct dev_gpio* self) {
    struct dev_gpio_impl* this = (struct dev_gpio_impl*) self;
    HAL_GPIO_WritePin(this->port,this->pin, GPIO_PIN_RESET);
}

/*********************************************************************************************************
*   read the pin
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
static uint8_t read(struct dev_gpio* self) {
    struct dev_gpio_impl* this = (struct dev_gpio_impl*) self;
    GPIO_PinState status=HAL_GPIO_ReadPin(this->port,this->pin);
    if (status == GPIO_PIN_SET) return 1;
    else return 0;
}

/*********************************************************************************************************
*  toggle pin
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
static void toggle(struct dev_gpio* self) {
    struct dev_gpio_impl* this = (struct dev_gpio_impl*) self;
    HAL_GPIO_TogglePin(this->port,this->pin);
}

/*********************************************************************************************************
*   write the pin
*
*   @param   self  the gpio dev
*   @param   on_off  the switch
*   @return  void
*   @note
*********************************************************************************************************/
static void write(struct dev_gpio* self,uint8_t on_off) {
    if (on_off)  set_up(self);
    else        set_down(self);
}

/*********************************************************************************************************
*   led turn on
*
*   @param   self  the gpio dev
*   @return  void
*   @note
*********************************************************************************************************/
static void led_on(struct dev_gpio* self) {
    struct dev_gpio_impl* this = (struct dev_gpio_impl*) self;
    HAL_GPIO_WritePin(this->port,this->pin, this->led_active_level);
}

/*********************************************************************************************************
*   led turn off
*
*   @param   self  the gpio dev
*   @return  void
*   @note
*********************************************************************************************************/
static void led_off(struct dev_gpio* self) {
    struct dev_gpio_impl* this = (struct dev_gpio_impl*) self;
    HAL_GPIO_WritePin(this->port,this->pin, 1-this->led_active_level);
}

/*********************************************************************************************************
*   is led turn on
*
*   @param   self  the gpio dev
*   @return  bool
*   @note
*********************************************************************************************************/
static uint8_t is_led_on(struct dev_gpio* self) {
    struct dev_gpio_impl* this = (struct dev_gpio_impl*) self;

    if (read(self) == this->led_active_level) return 1;

    return 0;
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

    // printf("The GPIO_NUM is %d,s_cnt is %d\r\n",GPIO_NUM,s_cnt);
    BSP_Assert(s_cnt < GPIO_NUM,"Fail to register the GPIO dev",s_cnt);

    struct dev_gpio_impl* obj = &s_dev_gpio_list[s_cnt++];
    dev_gpio_conf* gpio_conf = (dev_gpio_conf*)conf;

    obj->dev.vt = &s_gpio_vt;
    obj->pin = gpio_conf->pin;
    obj->port = gpio_conf->port;
    obj->dev_id = s_cnt-1;
    obj->led_active_level = gpio_conf->led_active_level;
}

/*********************************************************************************************************
*   get the gpio dev
*
*   @param   obj  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
void GPIO_DevGet(struct dev_gpio** obj,uint8_t ind) {
    BSP_Assert(ind < GPIO_NUM,"Fail to get the GPIO dev",ind);

    *obj = (struct dev_gpio*)(&s_dev_gpio_list[ind]); // 赋值

}

