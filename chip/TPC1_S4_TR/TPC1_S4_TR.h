/*********************************************************************************************************
*	TPC116S4-TR/TPC112S4-TR Driver
*
*	@brief  TPC116S4-TR/TPC112S4-TR is a 4-channel  digital-to-analog converters(DAC) with 12-bit resolution
*			for TPC112S4-TR while 16-bit for TPC116S4-TR.
*   @author   Created by Chanlin on 2026/7/16.
*   @version  1.0
*   @update   
*********************************************************************************************************/
#ifndef BSP_CHIP_TPC1S4TR_H
#define BSP_CHIP_TPC1S4TR_H

/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include "bsp_conf.h"
#include <stdint.h>

/*********************************************************************************************************
*                                               Public Macro
*********************************************************************************************************/

/*********************************************************************************************************
*                                               Enum
*********************************************************************************************************/
typedef enum {
	CHIP_TPC1S4TR_CHANNEL_A = 0,
	CHIP_TPC1S4TR_CHANNEL_B ,
	CHIP_TPC1S4TR_CHANNEL_C ,
	CHIP_TPC1S4TR_CHANNEL_D ,
	CHIP_TPC1S4TR_CHANNEL_MAX ,
}chip_tpc1s4tr_channel_t;

/*********************************************************************************************************
*                                               Public Structure
*********************************************************************************************************/
struct tpc1s4tr_t;

struct chip_tpc1s4tr_vt
{
	void (*write)(struct tpc1s4tr_t* self,chip_tpc1s4tr_channel_t channel,uint16_t data);
	void (*enable_cs)(struct tpc1s4tr_t* self);
	void (*disable_cs)(struct tpc1s4tr_t* self);
	
	void (*set_output)(struct tpc1s4tr_t* self,chip_tpc1s4tr_channel_t channel,double out_put);
};

typedef struct tpc1s4tr_t {
	struct chip_tpc1s4tr_vt* vt;
}chip_tpc1s4tr_t;

typedef struct tpc1s4tr_config_t {
	double ref;
	struct dev_spi* ptr_dev_spi;
	struct dev_gpio* load_pin;
	uint8_t chip_type;	// 1:116 0:112
}chip_tpc1s4tr_config_t;

/*********************************************************************************************************
*                                               API List
*********************************************************************************************************/
void TPC1S4_DevRegister(void* conf);
void TPC1S4_DevGet(struct tpc1s4tr_t** obj,uint8_t ind);


#endif //FACIAL_STIMULATION_DEVICE_TPC116S4_TR_H
