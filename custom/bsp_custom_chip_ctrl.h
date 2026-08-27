/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update
*********************************************************************************************************/
#ifndef __BSP_CUSTOM_CHIP_CTRL_H__
#define __BSP_CUSTOM_CHIP_CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include "bsp_conf.h"
#include "bsp_gpio.h"

/*********************************************************************************************************
*                                               Public Declaration
*********************************************************************************************************/
typedef enum {
    ENUM_STIM_CTRL_OFF=0,
    ENUM_STIM_CTRL_STIM,
    ENUM_STIM_CTRL_H1,
    ENUM_STIM_CTRL_H1_OFF,

    ENUM_STIM_CTRL_H2,
    ENUM_STIM_CTRL_H2_OFF,

    ENUM_STIM_CTRL_H1_H2,
    ENUM_STIM_CTRL_MAX
}stim_chip_ctrl_level_t;


struct dev_custom_chip_ctrl;

struct dev_custom_chip_ctrl_vt
{
    void (*rise_gen)(struct dev_custom_chip_ctrl* self);
    void (*fall_gen)(struct dev_custom_chip_ctrl* self);

    void (*set_level)(struct dev_custom_chip_ctrl* self,
                      uint8_t level);
    stim_chip_ctrl_level_t (*get_level)(struct dev_custom_chip_ctrl* self);
};

struct dev_custom_chip_ctrl
{
    struct dev_custom_chip_ctrl_vt* vt;
};

typedef struct
{
    struct dev_gpio* begin;
    struct dev_gpio* stim;
    struct dev_gpio* h1;
    struct dev_gpio* h2;

} dev_custom_chip_ctrl_conf;

/*********************************************************************************************************
*                                               API
*********************************************************************************************************/
void CustomChipCtrl_DevRegister(void* conf);

void CustomChipCtrl_DevGet(struct dev_custom_chip_ctrl** obj,
                           uint8_t ind);

#ifdef __cplusplus
}
#endif

#endif