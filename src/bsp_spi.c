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

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_SPI_NUM
#define SPI_NUM DEV_SPI_NUM
#else
#define SPI_NUM 1
#endif

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_spi
{
    struct dev_spi_vt* vt;

    SPI_HandleTypeDef* hspi;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function decl
static void _transmit(struct dev_spi_vt* self,uint8_t* tx_buf,uint16_t len);
static void _receive(struct dev_spi_vt* self,uint8_t* rx_buf,uint16_t len);
static void _transfer(struct dev_spi_vt* self,uint8_t* tx_buf,uint8_t* rx_buf,uint16_t len);

// static virtual function list
static struct dev_spi s_dev_spi_list[SPI_NUM]={};

static struct dev_spi_vt s_spi_vt =
{
    .send = _transmit,
    .recv  = _receive,
    .send_recv = _transfer,
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/
/*********************************************************************************************************
*   send data
*
*   @param   self     the spi dev
*   @param   tx_buf   send buffer
*   @param   len      send length
*   @return  void
*   @note
*********************************************************************************************************/
static void _transmit(struct dev_spi_vt* self,uint8_t* tx_buf,uint16_t len)
{
    struct dev_spi* this = (struct dev_spi*)self;

    HAL_SPI_Transmit(this->hspi,
                     tx_buf,
                     len,
                     HAL_MAX_DELAY);
}

/*********************************************************************************************************
*   recv data
*
*   @param   self     the spi dev
*   @param   rx_buf   recv buffer
*   @param   len      recv length
*   @return  void
*   @note
*********************************************************************************************************/
static void _receive(struct dev_spi_vt* self,uint8_t* rx_buf,uint16_t len)
{
    struct dev_spi* this = (struct dev_spi*)self;

    HAL_SPI_Receive(this->hspi,
                    rx_buf,
                    len,
                    HAL_MAX_DELAY);
}

/*********************************************************************************************************
*   send_recv data
*
*   @param   self     the spi dev
*   @param   tx_buf   send buffer
*   @param   rx_buf   recv buffer
*   @param   len      send_recv length
*   @return  void
*   @note
*********************************************************************************************************/
static void _transfer(struct dev_spi_vt* self,
                      uint8_t* tx_buf,
                      uint8_t* rx_buf,
                      uint16_t len)
{
    struct dev_spi* this = (struct dev_spi*)self;

    HAL_SPI_TransmitReceive(this->hspi,
                            tx_buf,
                            rx_buf,
                            len,
                            HAL_MAX_DELAY);
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   init the spi dev with the conf
*
*   @param   conf     spi config
*   @return  void
*   @note
*********************************************************************************************************/
void SPI_DevRegister(void* conf)
{
    static uint8_t s_cnt=0;

    BSP_Assert(s_cnt < SPI_NUM,"Fail to register the SPI dev");


    struct dev_spi* obj = &s_dev_spi_list[s_cnt++];

    dev_spi_conf* spi_conf = (dev_spi_conf*)conf;

    obj->vt   = &s_spi_vt;
    obj->hspi = spi_conf->hspi;
}

/*********************************************************************************************************
*   get the spi dev
*
*   @param   obj      spi object
*   @param   ind      index
*   @return  void
*   @note
*********************************************************************************************************/
void SPI_DevGet(struct dev_spi_vt** obj,uint8_t ind)
{
    BSP_Assert(ind < SPI_NUM,"Fail to get the SPI dev");

    *obj = (struct dev_spi_vt*)&s_dev_spi_list[ind];
}
