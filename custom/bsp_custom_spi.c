/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Header
*********************************************************************************************************/
#include "bsp_custom_spi.h"
#include "bsp_delay.h"

#include <stdio.h>

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_CUSTOM_SPI_NUM
#define CUSTOM_SPI_NUM DEV_CUSTOM_SPI_NUM
#else
#define CUSTOM_SPI_NUM 1
#endif

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_custom_spi_impl
{
    struct dev_custom_spi dev;

    struct dev_gpio* gen;
    struct dev_gpio* cs;
    struct dev_gpio* clk;
    struct dev_gpio* data;
    struct dev_gpio* load;

    uint32_t delay_us;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
static void _reset(struct dev_custom_spi* self);

static void _rise_gen(struct dev_custom_spi* self);
static void _fall_gen(struct  dev_custom_spi* self);

static void _cs_enable(struct dev_custom_spi* self);
static void _cs_disable(struct dev_custom_spi* self);

static void _rise_clk(struct dev_custom_spi* self);
static void _fall_clk(struct dev_custom_spi* self);

static void _set_data(struct dev_custom_spi* self,
                      uint8_t bit);

static void _rise_load(struct dev_custom_spi* self);
static void _fall_load(struct dev_custom_spi* self);

/*********************************************************************************************************
*                                              Static Variable
*********************************************************************************************************/
static struct dev_custom_spi_impl
    s_dev_custom_spi_list[CUSTOM_SPI_NUM] = {};

static struct dev_custom_spi_vt s_custom_spi_vt =
{
    // .reset      = _reset,

    .rise_gen=_rise_gen,
    .fall_gen=_fall_gen,

    .cs_enable  = _cs_enable,
    .cs_disable = _cs_disable,

    .rise_clk   = _rise_clk,
    .fall_clk   = _fall_clk,

    .set_data   = _set_data,

    .rise_load  = _rise_load,
    .fall_load  = _fall_load,
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/

/*********************************************************************************************************
*   protocol reset
*
*   GEN low active
*********************************************************************************************************/
static void _reset(struct dev_custom_spi* self)
{
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    this->gen->vt->set_down(this->gen);

    delay_us(this->delay_us);

    this->gen->vt->set_up(this->gen);

    delay_us(this->delay_us);
}

static void _rise_gen(struct dev_custom_spi* self) {
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    this->gen->vt->set_up(this->gen);
}

static void _fall_gen(struct  dev_custom_spi* self) {
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    this->gen->vt->set_down(this->gen);
}

/*********************************************************************************************************
*   enable cs
*********************************************************************************************************/
static void _cs_enable(struct dev_custom_spi* self)
{
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    this->cs->vt->set_up(this->cs);
}

/*********************************************************************************************************
*   disable cs
*********************************************************************************************************/
static void _cs_disable(struct dev_custom_spi* self)
{
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    this->cs->vt->set_down(this->cs);
}

/*********************************************************************************************************
*   clock rising edge
*********************************************************************************************************/
static void _rise_clk(struct dev_custom_spi* self)
{
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    this->clk->vt->set_up(this->clk);
}

/*********************************************************************************************************
*   clock falling edge
*********************************************************************************************************/
static void _fall_clk(struct dev_custom_spi* self)
{
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    this->clk->vt->set_down(this->clk);
}

/*********************************************************************************************************
*   set data bit
*********************************************************************************************************/
static void _set_data(struct dev_custom_spi* self,
                      uint8_t bit)
{
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    if(bit)
    {
        this->data->vt->set_up(this->data);
    }
    else
    {
        this->data->vt->set_down(this->data);
    }
}

/*********************************************************************************************************
*   load rising edge
*********************************************************************************************************/
static void _rise_load(struct dev_custom_spi* self)
{
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    this->load->vt->set_up(this->load);
}

/*********************************************************************************************************
*   load falling edge
*********************************************************************************************************/
static void _fall_load(struct dev_custom_spi* self)
{
    struct dev_custom_spi_impl* this =
        (struct dev_custom_spi_impl*)self;

    this->load->vt->set_down(this->load);
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
void CustomSPI_DevRegister(void* conf)
{
    static uint8_t s_cnt = 0;

    BSP_Assert(s_cnt < CUSTOM_SPI_NUM,
               "Fail to register the custom spi",s_cnt);

    struct dev_custom_spi_impl* obj =
        &s_dev_custom_spi_list[s_cnt++];

    dev_custom_spi_conf* spi_conf =
        (dev_custom_spi_conf*)conf;

    obj->dev.vt = &s_custom_spi_vt;

    obj->gen  = spi_conf->gen;
    obj->cs   = spi_conf->cs;
    obj->clk  = spi_conf->clk;
    obj->data = spi_conf->data;
    obj->load = spi_conf->load;

    obj->delay_us = spi_conf->delay_us;

    /* protocol idle state */

    obj->gen->vt->set_down(obj->gen);

    obj->cs->vt->set_down(obj->cs);

    obj->clk->vt->set_down(obj->clk);

    obj->data->vt->set_down(obj->data);

    obj->load->vt->set_down(obj->load);
}

void CustomSPI_DevGet(struct dev_custom_spi** obj,
                      uint8_t ind)
{
    BSP_Assert(ind < CUSTOM_SPI_NUM,
               "Fail to get the custom spi",ind);

    *obj =
        (struct dev_custom_spi*)
        &s_dev_custom_spi_list[ind];
}