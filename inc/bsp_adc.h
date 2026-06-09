#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "bsp_conf.h"

typedef uint16_t ad_val;

#define ADC1ChannelNum 1
#define ADC2ChannelNum 1


struct dev_adc_vt;

struct dev_adc_vt
{
  void (*start)(struct dev_adc_vt* self);
  void (*stop)(struct dev_adc_vt* self);
  uint16_t (*getValue)(struct dev_adc_vt* self,int channel);
};

typedef struct
{ 
  ADC_HandleTypeDef* hadc;
}dev_adc_conf;

void ADC_DevRegister(void* conf);
void ADC_DevGet(struct dev_adc_vt** obj,uint8_t ind);


#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

