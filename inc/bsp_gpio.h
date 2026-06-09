#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_conf.h"

struct dev_gpio_vt;
struct dev_gpio_vt
{
    void (*set_up)(struct dev_gpio_vt* self);
    void (*set_down)(struct dev_gpio_vt* self);
    void (*toggle)(struct dev_gpio_vt* self);
    uint8_t (*read)(struct dev_gpio_vt* self);
};

typedef struct
{
    GPIO_TypeDef* port;
    uint32_t pin;
}dev_gpio_conf;


void GPIO_DevRegister(void* conf);
void GPIO_DevGet(struct dev_gpio_vt** obj,uint8_t ind);

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

