/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Header
*********************************************************************************************************/
#include "bsp_adc.h"
#include <stdio.h>

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_ADC_NUM
#define ADC_NUM  DEV_ADC_NUM
#else
#define ADC_NUM  1
#endif

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_adc_impl
{
    struct dev_adc dev;           // 公共接口（放在第一个）

    ADC_HandleTypeDef* hadc;
    ad_val* buffer;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void enable(struct dev_adc* self);
static void disable(struct dev_adc* self);
static ad_val get_value(struct dev_adc* self, uint8_t channel);

// static virtual function list
static struct dev_adc_impl s_dev_adc_list[ADC_NUM] = {};

static ad_val s_adc1_buffer[ADC1_USED_CHANNEL_NUM] = {};
static ad_val s_adc2_buffer[ADC2_USED_CHANNEL_NUM] = {};

static struct dev_adc_vt s_adc_vt = {
    .start     = enable,
    .stop      = disable,
    .get_value = get_value,
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/
/*********************************************************************************************************
*   start adc dma convert
*
*   @param   self  the adc dev
*   @return  void
*   @note
*********************************************************************************************************/
static void enable(struct dev_adc* self)
{
    struct dev_adc_impl* this = (struct dev_adc_impl*)self;
    uint8_t channel_num = 0;

    if (this == NULL || this->hadc == NULL)
        return;

    if (this->hadc->Instance == ADC1)
        channel_num = ADC1_USED_CHANNEL_NUM;
    else if (this->hadc->Instance == ADC2)
        channel_num = ADC2_USED_CHANNEL_NUM;
    else
        return;

    HAL_ADC_Start_DMA(this->hadc, (uint32_t*)this->buffer, channel_num);
}

/*********************************************************************************************************
*   stop adc dma convert
*
*   @param   self  the adc dev
*   @return  void
*   @note
*********************************************************************************************************/
static void disable(struct dev_adc* self)
{
    struct dev_adc_impl* this = (struct dev_adc_impl*)self;

    if (this == NULL || this->hadc == NULL)
        return;

    HAL_ADC_Stop_DMA(this->hadc);
}

/*********************************************************************************************************
*   get adc value by channel
*
*   @param   self     the adc dev
*   @param   channel  channel index
*   @return  adc value
*   @note
*********************************************************************************************************/
static ad_val get_value(struct dev_adc* self, uint8_t channel)
{
    struct dev_adc_impl* this = (struct dev_adc_impl*)self;

    if (this == NULL || this->hadc == NULL || this->buffer == NULL)
        return 123;

    if (this->hadc->Instance == ADC1) {
        if (channel < ADC1_USED_CHANNEL_NUM)
            return *(this->buffer + channel);
        else
            return 123;
    }
    else if (this->hadc->Instance == ADC2) {
        if (channel < ADC2_USED_CHANNEL_NUM)
            return *(this->buffer + channel);
        else
            return 123;
    }
    else
        return 123;
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   register the adc device
*
*   @param   conf  the adc config
*   @return  void
*   @note
*********************************************************************************************************/
void ADC_DevRegister(void* conf)
{
    static uint8_t s_cnt = 0;

    BSP_Assert(s_cnt < ADC_NUM, "Fail to register the ADC dev",s_cnt);

    struct dev_adc_impl* obj = &s_dev_adc_list[s_cnt++];
    dev_adc_conf* adc_conf = (dev_adc_conf*)conf;

    obj->dev.vt   = &s_adc_vt;
    obj->hadc     = adc_conf->hadc;

    if (obj->hadc->Instance == ADC1)
        obj->buffer = s_adc1_buffer;
    else if (obj->hadc->Instance == ADC2)
        obj->buffer = s_adc2_buffer;
    else
        return;
}

/*********************************************************************************************************
*   get the adc device
*
*   @param   obj   the adc device pointer (output)
*   @param   ind   device index
*   @return  void
*   @note
*********************************************************************************************************/
void ADC_DevGet(struct dev_adc** obj, uint8_t ind)
{
    BSP_Assert(ind < ADC_NUM, "Fail to get the ADC dev",ind);

    *obj = (struct dev_adc*)&s_dev_adc_list[ind];
}