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

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_adc {
  struct dev_adc_vt* vt;
  ADC_HandleTypeDef* hadc;
  ad_val* buffer;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
static struct dev_adc s_adc_list[2]={};

ad_val s_adc1_buffer[ADC1_USED_CHANNEL_NUM]={};
ad_val s_adc2_buffer[ADC2_USED_CHANNEL_NUM]={};

// static function declaration
static void _adc_start(struct dev_adc_vt* self);
static void _adc_stop(struct dev_adc_vt* self);
static uint16_t _adc_getValue(struct dev_adc_vt* self,uint8_t channel);

// static virtual function list
static struct dev_adc_vt s_adc_vt ={
  .start = _adc_start,
  .stop = _adc_stop,
  .getValue = _adc_getValue,
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/
/*********************************************************************************************************
*   start adc dma convert
*
*   @param   self      the adc dev
*   @return  void
*   @note
*********************************************************************************************************/
static void _adc_start(struct dev_adc_vt* self)
{
  struct dev_adc* this = (struct dev_adc*)self;
  uint8_t channel_num=0;
  if (this == NULL || this->hadc == NULL)
    return;
  if (this->hadc->Instance == ADC1) channel_num = ADC1_USED_CHANNEL_NUM;
  else if (this->hadc->Instance == ADC2)channel_num = ADC2_USED_CHANNEL_NUM;
  else return;
    HAL_ADC_Start_DMA(this->hadc, (uint32_t*)this->buffer, channel_num);
}

/*********************************************************************************************************
*   stop adc dma convert
*
*   @param   self      the adc dev
*   @return  void
*   @note
*********************************************************************************************************/
static void _adc_stop(struct dev_adc_vt* self)
{
  struct dev_adc* adc = (struct dev_adc*)self;

  if (adc == NULL || adc->hadc == NULL)
    return;

  HAL_ADC_Stop_DMA(adc->hadc);
}

/*********************************************************************************************************
*   get adc value by channel
*
*   @param   self      the adc dev
*   @param   channel   channel index
*   @return  adc value
*   @note
*********************************************************************************************************/
static uint16_t _adc_getValue(struct dev_adc_vt* self, uint8_t channel)
{
  struct dev_adc* this = (struct dev_adc*)self;

  if (this == NULL || this->hadc == NULL || this->buffer == NULL)
    return  123;

  if (this->hadc->Instance == ADC1) {
    if (channel < ADC1_USED_CHANNEL_NUM) return *(this->buffer + channel);
    else 123;
  }
  else if (this->hadc->Instance == ADC2) {
    if (channel < ADC2_USED_CHANNEL_NUM) return *(this->buffer + channel);
    else 123;
  }
  else return 123;
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   init the adc dev with the conf
*
*   @param   conf      adc config
*   @return  void
*   @note
*********************************************************************************************************/
void ADC_DevRegister(void* conf) {
  dev_adc_conf* config=(dev_adc_conf*)conf;
  static uint8_t s_cnt=0;

  BSP_Assert(ind <DEV_ADC_NUM);
  struct dev_adc* obj = &s_adc_list[s_cnt++];

  obj->vt = &s_adc_vt;
  obj->hadc = config->hadc;
  if (obj->hadc->Instance == ADC1 ) obj->buffer = s_adc1_buffer;
  else if (obj->hadc->Instance == ADC2 ) obj->buffer = s_adc2_buffer;
  else return;
}

/*********************************************************************************************************
*   get the adc dev
*
*   @param   obj       adc object
*   @param   ind       device index
*   @return  void
*   @note
*********************************************************************************************************/
void ADC_DevGet(struct dev_adc_vt** obj,uint8_t ind) {
  BSP_Assert(ind <DEV_ADC_NUM);

  *obj = (struct dev_adc_vt*) &s_adc_list[ind];
}

