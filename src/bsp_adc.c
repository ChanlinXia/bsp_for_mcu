
#include "bsp_adc.h"


struct dev_adc {
  struct dev_adc_vt* vt;
  ADC_HandleTypeDef* hadc;
  ad_val* buffer;
};

static struct dev_adc s_adc_list[2]={};

ad_val s_adc1_buffer[ADC1ChannelNum]={};
ad_val s_adc2_buffer[ADC2ChannelNum]={};

// 虚函数列表
static void _adc_start(struct dev_adc_vt* self);
static void _adc_stop(struct dev_adc_vt* self);
static uint16_t _adc_getValue(struct dev_adc_vt* self,int channel);

// 静态虚函数
static struct dev_adc_vt s_adc_vt ={
  .start = _adc_start,
  .stop = _adc_stop,
  .getValue = _adc_getValue,
};


static void _adc_start(struct dev_adc_vt* self)
{
  struct dev_adc* this = (struct dev_adc*)self;
  int channel_num=0;
  if (this == NULL || this->hadc == NULL)
    return;
  if (this->hadc->Instance == ADC1) channel_num = ADC1ChannelNum;
  else if (this->hadc->Instance == ADC2)channel_num = ADC2ChannelNum;
  else ;
    HAL_ADC_Start_DMA(this->hadc, (uint32_t*)this->buffer, channel_num);
}

static void _adc_stop(struct dev_adc_vt* self)
{
  struct dev_adc* adc = (struct dev_adc*)self;

  if (adc == NULL || adc->hadc == NULL)
    return;

  HAL_ADC_Stop_DMA(adc->hadc);
}

static uint16_t _adc_getValue(struct dev_adc_vt* self, int channel)
{
  struct dev_adc* this = (struct dev_adc*)self;

  if (this == NULL || this->hadc == NULL || this->buffer == NULL)
    return  123;

  if (this->hadc->Instance == ADC1) {
    if (channel >= 0 && channel < ADC1ChannelNum) return *(this->buffer + channel);
    else 123;
  }
  else if (this->hadc->Instance == ADC2) {
    if (channel >= 0 && channel < ADC2ChannelNum) return *(this->buffer + channel);
    else 123;
  }
  else ;
}


void ADC_DevRegister(void* conf) {
  dev_adc_conf* config=(dev_adc_conf*)conf;
  static uint8_t s_cnt=0;
  struct dev_adc* obj = &s_adc_list[s_cnt++];

  obj->vt = &s_adc_vt;
  obj->hadc = config->hadc;
  if (obj->hadc->Instance == ADC1 ) obj->buffer = s_adc1_buffer;
  else if (obj->hadc->Instance == ADC2 ) obj->buffer = s_adc2_buffer;
  else ;
}

void ADC_DevGet(struct dev_adc_vt** obj,uint8_t ind) {
  Assert(ind >DEV_ADC_NUM);

  *obj = (struct dev_adc_vt*) &s_adc_list[ind];
}


