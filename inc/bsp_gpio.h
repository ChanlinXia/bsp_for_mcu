/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update   
*********************************************************************************************************/
#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif
/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include "bsp_conf.h"

/*********************************************************************************************************
*                                               Public Macro
*********************************************************************************************************/


/*********************************************************************************************************
*                                               Public Declaration
*********************************************************************************************************/
struct dev_gpio;
struct dev_gpio_vt
{
  // gpio功能
  void (*set_up)(struct dev_gpio* self);
  void (*set_down)(struct dev_gpio* self);
  void (*write)(struct dev_gpio* self, uint8_t on_off);
  void (*toggle)(struct dev_gpio* self);
  uint8_t (*read)(struct dev_gpio* self);

  // led功能
  void (*led_on)(struct dev_gpio* self);
  void (*led_off)(struct dev_gpio* self);
  uint8_t (*is_led_on)(struct dev_gpio* self);
};

struct dev_gpio {
  // public 变量待补充
  struct dev_gpio_vt* vt;
};

typedef struct
{ 
  GPIO_TypeDef* port;
  uint32_t pin;
  uint8_t led_active_level; // led
}dev_gpio_conf;

/*********************************************************************************************************
*                                               API
*********************************************************************************************************/
void GPIO_DevRegister(void* conf);
void GPIO_DevGet(struct dev_gpio** obj,uint8_t ind);

#ifdef __cplusplus
}
#endif

#endif /* BSP_GPIO_H */

