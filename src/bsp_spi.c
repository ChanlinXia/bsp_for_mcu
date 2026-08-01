/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update
*********************************************************************************************************/
/*********************************************************************************************************
*                                              Header
*********************************************************************************************************/
#include "bsp_spi.h"
#include <stdio.h>

#include "bsp_gpio.h"

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_SPI_NUM

#define SPI_NUM  DEV_SPI_NUM
#else
#define SPI_NUM  1
#endif

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_spi_impl
{
    struct dev_spi dev;           // 公共接口（放在第一个）

    SPI_HandleTypeDef* hspi;      // SPI 句柄
    uint8_t use_soft_cs;
    struct dev_gpio* cs_pin;
    uint8_t dev_id;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void transmit(struct dev_spi* self, uint8_t* tx_buf, uint16_t len);
static void receive(struct dev_spi* self, uint8_t* rx_buf, uint16_t len);
static void transfer(struct dev_spi* self, uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len);
static void rise_cs(struct dev_spi* self);
static void fall_cs(struct dev_spi* self);

// static virtual function list
static struct dev_spi_impl s_dev_spi_list[SPI_NUM] = {};
static struct dev_spi_vt s_spi_vt = {
    .transmit = transmit,
    .receive  = receive,
    .transfer = transfer,
    .rise_cs = rise_cs,
    .fall_cs = fall_cs
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/
/*********************************************************************************************************
*   send data
*
*   @param   self    the spi dev
*   @param   tx_buf  send buffer
*   @param   len     send length
*   @return  void
*   @note
*********************************************************************************************************/
static void transmit(struct dev_spi* self, uint8_t* tx_buf, uint16_t len)
{
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;

    BSP_Assert(HAL_OK==HAL_SPI_Transmit(this->hspi, tx_buf, len, HAL_MAX_DELAY)
                ,"bsp spi fail to transmit data",this->dev_id);

    // if () {
    //     // BSP_Assert()
    // }
}

/*********************************************************************************************************
*   receive data
*
*   @param   self    the spi dev
*   @param   rx_buf  receive buffer
*   @param   len     receive length
*   @return  void
*   @note
*********************************************************************************************************/
static void receive(struct dev_spi* self, uint8_t* rx_buf, uint16_t len)
{
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;

    HAL_SPI_Receive(this->hspi, rx_buf, len, HAL_MAX_DELAY);
}

/*********************************************************************************************************
*   send and receive data
*
*   @param   self    the spi dev
*   @param   tx_buf  send buffer
*   @param   rx_buf  receive buffer
*   @param   len     transfer length
*   @return  void
*   @note
*********************************************************************************************************/
static void transfer(struct dev_spi* self, uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len)
{
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;

    HAL_SPI_TransmitReceive(this->hspi, tx_buf, rx_buf, len, HAL_MAX_DELAY);
}

/*********************************************************************************************************
*   pull up the cs pin
*
*   @param   self    the spi dev
*   @return  void
*   @note
*********************************************************************************************************/
static void rise_cs(struct dev_spi* self) {
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;
    if (!(this->use_soft_cs)) return;
    this->cs_pin->vt->set_up(this->cs_pin);
}

/*********************************************************************************************************
*   pull down the cs pin
*
*   @param   self    the spi dev
*   @return  void
*   @note
*********************************************************************************************************/
static void fall_cs(struct dev_spi* self) {
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;
    if (!(this->use_soft_cs)) return;

    this->cs_pin->vt->set_down(this->cs_pin);
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   register the spi device
*
*   @param   conf  the spi config
*   @return  void
*   @note
*********************************************************************************************************/
void SPI_DevRegister(void* conf)
{
    static uint8_t s_cnt = 0;

    BSP_Assert(s_cnt < SPI_NUM, "Fail to register the SPI dev",s_cnt);

    struct dev_spi_impl* obj = &s_dev_spi_list[s_cnt++];
    dev_spi_conf* spi_conf = (dev_spi_conf*)conf;

    obj->dev.vt   = &s_spi_vt;
    obj->hspi     = spi_conf->hspi;

    if (spi_conf->use_soft_cs) {
        obj->use_soft_cs = 1;
        obj->cs_pin = spi_conf->cs_pin;
    }

    obj->dev_id = s_cnt-1;
}

/*********************************************************************************************************
*   get the spi device
*
*   @param   obj   the spi device pointer (output)
*   @param   ind   device index
*   @return  void
*   @note
*********************************************************************************************************/
void SPI_DevGet(struct dev_spi** obj, uint8_t ind)
{
    BSP_Assert(ind < SPI_NUM, "Fail to get the SPI dev",ind);

    *obj = (struct dev_spi*)&s_dev_spi_list[ind];
}