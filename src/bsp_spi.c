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
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void _transmit(struct dev_spi* self, uint8_t* tx_buf, uint16_t len);
static void _receive(struct dev_spi* self, uint8_t* rx_buf, uint16_t len);
static void _transfer(struct dev_spi* self, uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len);

// static virtual function list
static struct dev_spi_impl s_dev_spi_list[SPI_NUM] = {};
static struct dev_spi_vt s_spi_vt = {
    .transmit = _transmit,
    .receive  = _receive,
    .transfer = _transfer,
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
static void _transmit(struct dev_spi* self, uint8_t* tx_buf, uint16_t len)
{
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;

    HAL_SPI_Transmit(this->hspi, tx_buf, len, HAL_MAX_DELAY);
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
static void _receive(struct dev_spi* self, uint8_t* rx_buf, uint16_t len)
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
static void _transfer(struct dev_spi* self, uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len)
{
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;

    HAL_SPI_TransmitReceive(this->hspi, tx_buf, rx_buf, len, HAL_MAX_DELAY);
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

    BSP_Assert(s_cnt < SPI_NUM, "Fail to register the SPI dev");

    struct dev_spi_impl* obj = &s_dev_spi_list[s_cnt++];
    dev_spi_conf* spi_conf = (dev_spi_conf*)conf;

    obj->dev.vt   = &s_spi_vt;
    obj->hspi     = spi_conf->hspi;
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
    BSP_Assert(ind < SPI_NUM, "Fail to get the SPI dev");

    *obj = (struct dev_spi*)&s_dev_spi_list[ind];
}