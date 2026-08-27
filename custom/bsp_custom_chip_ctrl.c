/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Header
*********************************************************************************************************/
#include "bsp_custom_chip_ctrl.h"
#include <stdio.h>

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_CUSTOM_CHIP_CTRL_NUM

#define CUSTOM_CHIP_CTRL_NUM DEV_CUSTOM_CHIP_CTRL_NUM

#else

#define CUSTOM_CHIP_CTRL_NUM 1

#endif

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_custom_chip_ctrl_impl
{
    struct dev_custom_chip_ctrl dev;

    struct dev_gpio* begin;
    struct dev_gpio* stim;
    struct dev_gpio* h1;
    struct dev_gpio* h2;

    stim_chip_ctrl_level_t cur_level;

};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
static void _rise_gen(struct dev_custom_chip_ctrl* self);

static void _fall_gen(struct dev_custom_chip_ctrl* self);

static void _set_level(struct dev_custom_chip_ctrl* self,
                       uint8_t level);

static stim_chip_ctrl_level_t _get_level(struct dev_custom_chip_ctrl* self);

static struct dev_custom_chip_ctrl_impl
    s_dev_custom_chip_ctrl_list[CUSTOM_CHIP_CTRL_NUM] = {};

static struct dev_custom_chip_ctrl_vt
    s_custom_chip_ctrl_vt =
{
    .rise_gen  = _rise_gen,
    .fall_gen  = _fall_gen,
    .set_level = _set_level,
    .get_level = _get_level,
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/
static stim_chip_ctrl_level_t _get_level(struct dev_custom_chip_ctrl* self) {
    struct dev_custom_chip_ctrl_impl* this =
        (struct dev_custom_chip_ctrl_impl*)self;

    return this->cur_level;
}

/*********************************************************************************************************
*   enable stimulation
*********************************************************************************************************/
static void _rise_gen(struct dev_custom_chip_ctrl* self)
{
    struct dev_custom_chip_ctrl_impl* this =
        (struct dev_custom_chip_ctrl_impl*)self;

    this->begin->vt->set_up(this->begin);
}

/*********************************************************************************************************
*   disable stimulation
*********************************************************************************************************/
static void _fall_gen(struct dev_custom_chip_ctrl* self)
{
    struct dev_custom_chip_ctrl_impl* this =
        (struct dev_custom_chip_ctrl_impl*)self;

    this->begin->vt->set_down(this->begin);

    this->stim->vt->set_down(this->stim);
    this->h1->vt->set_down(this->h1);
    this->h2->vt->set_down(this->h2);
}

/*********************************************************************************************************
*   set stimulation level
*
*   level:
*       0 : off
*       1 : stim
*       2 : stim + h1
*       3 : stim + h2
*       4 : stim + h1 + h2
*********************************************************************************************************/
static void _set_level(struct dev_custom_chip_ctrl* self,
                       uint8_t level)
{
    struct dev_custom_chip_ctrl_impl* this =
        (struct dev_custom_chip_ctrl_impl*)self;
    this->cur_level = level;

    switch(level)
    {
        case ENUM_STIM_CTRL_H1_OFF:
        case ENUM_STIM_CTRL_H2_OFF:
        case ENUM_STIM_CTRL_OFF:
        {
            this->stim->vt->set_down(this->stim);
            this->h1->vt->set_down(this->h1);
            this->h2->vt->set_down(this->h2);
        }
        break;

        case ENUM_STIM_CTRL_STIM:
        {
            this->stim->vt->set_up(this->stim);

            this->h1->vt->set_down(this->h1);
            this->h2->vt->set_down(this->h2);
        }
        break;

        case ENUM_STIM_CTRL_H1:
        {
            this->stim->vt->set_up(this->stim);

            this->h1->vt->set_up(this->h1);
            this->h2->vt->set_down(this->h2);
        }
        break;

        case ENUM_STIM_CTRL_H2:
        {
            this->stim->vt->set_up(this->stim);

            this->h1->vt->set_down(this->h1);
            this->h2->vt->set_up(this->h2);
        }
        break;

        case ENUM_STIM_CTRL_H1_H2:
        {
            this->stim->vt->set_up(this->stim);

            this->h1->vt->set_up(this->h1);
            this->h2->vt->set_up(this->h2);
        }
        break;

        default:
        {
            BSP_Assert(0,
                       "Invalid custom chip ctrl level",level);
        }
        break;
    }
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
void CustomChipCtrl_DevRegister(void* conf)
{
    static uint8_t s_cnt = 0;

    BSP_Assert(s_cnt < CUSTOM_CHIP_CTRL_NUM,
               "Fail to register custom chip ctrl",s_cnt);

    struct dev_custom_chip_ctrl_impl* obj =
        &s_dev_custom_chip_ctrl_list[s_cnt++];

    dev_custom_chip_ctrl_conf* ctrl_conf =
        (dev_custom_chip_ctrl_conf*)conf;

    obj->dev.vt = &s_custom_chip_ctrl_vt;

    obj->begin = ctrl_conf->begin;
    obj->stim  = ctrl_conf->stim;
    obj->h1    = ctrl_conf->h1;
    obj->h2    = ctrl_conf->h2;
    obj->cur_level = ENUM_STIM_CTRL_OFF;

    obj->begin->vt->set_down(obj->begin);
    obj->stim->vt->set_down(obj->stim);
    obj->h1->vt->set_down(obj->h1);
    obj->h2->vt->set_down(obj->h2);
}

void CustomChipCtrl_DevGet(struct dev_custom_chip_ctrl** obj,
                           uint8_t ind)
{
    BSP_Assert(ind < CUSTOM_CHIP_CTRL_NUM,
               "Fail to get custom chip ctrl",ind);

    *obj =
        (struct dev_custom_chip_ctrl*)
        &s_dev_custom_chip_ctrl_list[ind];
}