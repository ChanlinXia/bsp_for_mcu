/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/24.
*   @version  1.0
*   @update
*********************************************************************************************************/
#ifndef __BSP_CUSTOM_SPI_H__
#define __BSP_CUSTOM_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include "bsp_conf.h"
#include "bsp_gpio.h"

/*********************************************************************************************************
*                                               Public Declaration
*********************************************************************************************************/
struct dev_custom_spi;

struct dev_custom_spi_vt
{
    // void (*reset)(struct dev_custom_spi* self);
    void (*rise_gen)(struct dev_custom_spi* self);
    void (*fall_gen)(struct dev_custom_spi* self);

    void (*end)(struct dev_custom_spi* self);

    void (*cs_enable)(struct dev_custom_spi* self);
    void (*cs_disable)(struct dev_custom_spi* self);
    void(*rise_clk)(struct dev_custom_spi* self);
    void(*fall_clk)(struct dev_custom_spi* self);
    void(*set_data)(struct dev_custom_spi* self,uint8_t bit);
    void(*rise_load)(struct dev_custom_spi* self);
    void(*fall_load)(struct dev_custom_spi* self);

};

struct dev_custom_spi
{
    struct dev_custom_spi_vt* vt;
};

typedef struct
{
    struct dev_gpio* gen;
    struct dev_gpio* cs;
    struct dev_gpio* clk;
    struct dev_gpio* data;
    struct dev_gpio* load;

    uint32_t delay_us;
} dev_custom_spi_conf;

/*********************************************************************************************************
*                                               API
*********************************************************************************************************/
void CustomSPI_DevRegister(void* conf);

void CustomSPI_DevGet(struct dev_custom_spi** obj,
                      uint8_t ind);

#ifdef __cplusplus
}
#endif

#endif