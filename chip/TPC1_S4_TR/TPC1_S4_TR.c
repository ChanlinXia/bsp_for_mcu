/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/7/16.
*   @version  1.0
*   @update   
*********************************************************************************************************/
/*********************************************************************************************************
*                                              Header File
*********************************************************************************************************/
#include "TPC1_S4_TR.h"

#include "bsp_gpio.h"
#include "bsp_spi.h"

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#if defined(CHIP_TPC1_S4_NUM)

#define CHIP_TPC1S4TR_NUM CHIP_TPC1_S4_NUM

#else

#define CHIP_TPC1S4TR_NUM 0

#endif

typedef enum {
	CHIP_CHANNEL_A_DATA = 0b1000,
	CHIP_CHANNEL_B_DATA = 0b1010,
	CHIP_CHANNEL_C_DATA = 0b1100,
	CHIP_CHANNEL_D_DATA = 0b1110,
}CHIP_CHANNEL_DATA;

/*********************************************************************************************************
*                                              Static Declaration and Variable
*********************************************************************************************************/
typedef struct chip_tpc1s4tr_impl
{
	struct tpc1s4tr_t chip;
	double ref;
	struct dev_spi* ptr_dev_spi;
	struct dev_gpio* n_en_pin;
	uint8_t chip_type;
	uint8_t chip_id;
}chip_tpc1s4tr_impl;

static void tpc116s4tr_write(struct tpc1s4tr_t* self,chip_tpc1s4tr_channel_t channel,uint8_t* data);
static void tpc112s4tr_write(struct tpc1s4tr_t* self,chip_tpc1s4tr_channel_t channel,uint8_t* data);
static uint8_t set_data(chip_tpc1s4tr_channel_t channel,uint8_t* data,uint8_t chip_type);
static void send_data(struct dev_spi* ptr_dev_spi,uint8_t* data,uint8_t data_len);

static void enable(struct tpc1s4tr_t* self);
static void disable(struct tpc1s4tr_t* self);

static struct chip_tpc1s4tr_vt s_tpc116s4tr_vt={
	.write = tpc116s4tr_write,
	.enable = enable,
	.disable = disable,
};

static struct chip_tpc1s4tr_vt s_tpc112s4tr_vt={
	.write = tpc112s4tr_write,
	.enable = enable,
	.disable = disable,
};

static chip_tpc1s4tr_impl s_chip_tpc1s4tr_list[CHIP_TPC1S4TR_NUM]={};

/*********************************************************************************************************
*                                              Static Function
*********************************************************************************************************/
/*********************************************************************************************************
*   send data
*
*   @param   self    the spi dev
*   @return  void
*   @note
*********************************************************************************************************/
static void enable(struct tpc1s4tr_t* self) {
	chip_tpc1s4tr_impl* this = (chip_tpc1s4tr_impl*)self;
	this->n_en_pin->vt->set_down(this->n_en_pin);
}

/*********************************************************************************************************
*   send data
*
*   @param   self    the spi dev
*   @return  void
*   @note
*********************************************************************************************************/
static void disable(struct tpc1s4tr_t* self) {
	chip_tpc1s4tr_impl* this = (chip_tpc1s4tr_impl*)self;
	this->n_en_pin->vt->set_up(this->n_en_pin);
}

/*********************************************************************************************************
*   send data
*
*   @param   self    the spi dev
*   @param   channel  send buffer
*   @param   data     send length
*   @return  void
*   @note
*********************************************************************************************************/
static void tpc116s4tr_write(struct tpc1s4tr_t* self,chip_tpc1s4tr_channel_t channel, uint8_t* data) {
	chip_tpc1s4tr_impl* this = (chip_tpc1s4tr_impl*)self;

	if (set_data(channel,data,this->chip_type)) {
		BSP_Assert(0,"tpc112s4tr fail to send data",this->chip_id);
		return;
	}

	send_data(this->ptr_dev_spi,data,3);
}

/*********************************************************************************************************
*   send data
*
*   @param   self    the spi dev
*   @param   channel  send buffer
*   @param   data     send length
*   @return  void
*   @note
*********************************************************************************************************/
static void tpc112s4tr_write(struct tpc1s4tr_t* self,chip_tpc1s4tr_channel_t channel, uint8_t* data) {
	chip_tpc1s4tr_impl* this = (chip_tpc1s4tr_impl*)self;

	if (set_data(channel,data,this->chip_type)) {
		BSP_Assert(0,"tpc112s4tr fail to send data",this->chip_id);
		return;
	}

	send_data(this->ptr_dev_spi,data,2);
}

/*********************************************************************************************************
*   send data
*
*   @param   ptr_dev_spi  the spi dev
*   @param   data		  send buffer
*   @param   data_len     send length
*   @return  void
*   @note
*********************************************************************************************************/
static void send_data(struct dev_spi* ptr_dev_spi,uint8_t* data,uint8_t data_len) {
	ptr_dev_spi->vt->fall_cs(ptr_dev_spi);

	ptr_dev_spi->vt->transmit(ptr_dev_spi,data,data_len);

	ptr_dev_spi->vt->rise_cs(ptr_dev_spi);
}

/*********************************************************************************************************
*   set data
*
*   @param channel	  channel id
*   @param   data     send data
*   @return  void
*   @note
*********************************************************************************************************/
static uint8_t set_data(chip_tpc1s4tr_channel_t channel,uint8_t* data,uint8_t chip_type){

	if (data == NULL)  return 1;

	// 1. 获取通道编码（根据你定义的宏）
	uint8_t channel_code;
	switch (channel) {
		case CHIP_TPC1S4TR_CHANNEL_A: channel_code = CHIP_CHANNEL_A_DATA; break;
		case CHIP_TPC1S4TR_CHANNEL_B: channel_code = CHIP_CHANNEL_B_DATA; break;
		case CHIP_TPC1S4TR_CHANNEL_C: channel_code = CHIP_CHANNEL_C_DATA; break;
		case CHIP_TPC1S4TR_CHANNEL_D: channel_code = CHIP_CHANNEL_D_DATA; break;
		default: return 0;
	}

	// 2. 获取原始 16 位数据（假设 data[0] 是高字节，data[1] 是低字节）
	uint16_t raw_value = ((uint16_t)data[0] << 8) | data[1];

	// 3. 根据芯片类型提取有效数据，并准备好通道码（只取低4位）
	uint8_t ch = channel_code & 0x0F;   // 通道码只占4位

	uint16_t dac_data;   // 存放有效数据（不左移，直接使用原始值）
	if (chip_type == 1) {
		// 116 模式：16位数据
		dac_data = raw_value & 0xFFFF;
		// 组装 24 位帧
		uint32_t frame = ((uint32_t)ch << 16) | dac_data;   // bit23~20 自动为0
		data[0] = (frame >> 16) & 0xFF;   // 高8位（含通道）
		data[1] = (frame >> 8)  & 0xFF;   // 数据高字节
		data[2] = frame & 0xFF;           // 数据低字节
		// 此时 data[0] 的内容 = 0x00 | ch （即高4位为0，低4位为通道）
		// 发送 3 字节
	} else {
		// 112 模式：12位数据
		dac_data = raw_value & 0x0FFF;
		// 组装 16 位帧
		uint16_t frame = ((uint16_t)ch << 12) | dac_data;   // bit15~12 = ch, bit11~0 = 数据
		data[0] = (frame >> 8) & 0xFF;   // 高字节（含通道）
		data[1] = frame & 0xFF;          // 低字节
		// 发送 2 字节
	}

	return 0;
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   pull up the pin
*
*   @param   conf  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
void TPC1S4_DevRegister(void* conf) {
	static uint8_t s_cnt=0;

	// printf("The GPIO_NUM is %d,s_cnt is %d\r\n",GPIO_NUM,s_cnt);
	BSP_Assert(s_cnt < CHIP_TPC1S4TR_NUM,"Fail to register the TPC1S4 chip",s_cnt);


	chip_tpc1s4tr_impl* obj = &s_chip_tpc1s4tr_list[s_cnt++];
	chip_tpc1s4tr_config_t* chip_conf = (chip_tpc1s4tr_config_t*)conf;

	obj->ref = chip_conf->ref;
	obj->chip_type = chip_conf->chip_type;

	if (chip_conf->chip_type == 1) {
		obj->chip.vt = &s_tpc116s4tr_vt;
	}
	else {
		obj->chip.vt = &s_tpc112s4tr_vt;
	}
	obj->chip_id = s_cnt-1;
	obj->ptr_dev_spi = chip_conf->ptr_dev_spi;
	obj->n_en_pin = chip_conf->n_en_pin;

}

/*********************************************************************************************************
*   send data
*
*   @param   obj    the spi dev
*   @param   ind  send buffer
*   @return  void
*   @note
*********************************************************************************************************/
void TPC1S4_DevGet(struct tpc1s4tr_t** obj,uint8_t ind) {
	BSP_Assert(ind < CHIP_TPC1S4TR_NUM,"Fail to register the TPC1S4 chip",ind);

	*obj = (struct tpc1s4tr_t*)(&s_chip_tpc1s4tr_list[ind]); // 赋值
}
