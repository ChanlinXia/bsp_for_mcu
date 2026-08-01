/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update
*********************************************************************************************************/
#ifndef BSP_SPI_H
#define BSP_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include "bsp_conf.h"

/*********************************************************************************************************
*                                               Public Macro
*********************************************************************************************************/

/*********************************************************************************************************
*                                               Public Declaration
*********************************************************************************************************/
struct dev_spi;

struct dev_spi_vt
{
    void (*rise_cs)(struct dev_spi* self);
    void (*fall_cs)(struct dev_spi* self);
    void (*transmit)(struct dev_spi* self, uint8_t* tx_buf, uint16_t len);
    void (*receive)(struct dev_spi* self, uint8_t* rx_buf, uint16_t len);
    void (*transfer)(struct dev_spi* self, uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len);
};

struct dev_spi
{
    struct dev_spi_vt* vt;
};

typedef struct
{
    SPI_HandleTypeDef* hspi;
    struct dev_gpio* cs_pin;
    uint8_t use_soft_cs;
} dev_spi_conf;

/*********************************************************************************************************
*                                               API
*********************************************************************************************************/
void SPI_DevRegister(void* conf);
void SPI_DevGet(struct dev_spi** obj, uint8_t ind);

#ifdef __cplusplus
}
#endif

#endif /* BSP_SPI_H */