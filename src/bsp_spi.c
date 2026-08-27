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
#include <string.h>

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

    dev_spi_conf cfg;

    // SPI_HandleTypeDef* hspi;      // SPI 句柄
    // uint8_t use_soft_cs;
    // struct dev_gpio* cs_pin;
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

static void soft_transmit(struct dev_spi* self, uint8_t* tx_buf, uint16_t len);
static void soft_recieve(struct dev_spi* self, uint8_t* rx_buf, uint16_t len);
static void soft_transfer(struct dev_spi* self, uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len);

// static virtual function list
static struct dev_spi_impl s_dev_spi_list[SPI_NUM] = {};
static struct dev_spi_vt s_spi_vt = {
    .transmit = transmit,
    .receive  = receive,
    .transfer = transfer,
    .rise_cs = rise_cs,
    .fall_cs = fall_cs
};

static struct dev_spi_vt s_soft_spi_vt = {
    .transmit = soft_transmit,
    .receive  = soft_recieve,
    .transfer = soft_transfer,
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

    BSP_Assert(HAL_OK==HAL_SPI_Transmit(this->cfg.hspi, tx_buf, len, HAL_MAX_DELAY)
                ,"bsp spi fail to transmit data",this->dev_id);

    // if () {
    //     // BSP_Assert()
    // }
}


/*********************************************************************************************************
*   send data by software
*
*   @param   self    the spi dev
*   @param   tx_buf  send buffer
*   @param   len     send length
*   @return  void
*   @note
*********************************************************************************************************/
static void soft_transmit(struct dev_spi* self, uint8_t* tx_buf, uint16_t len)
{
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;
    struct dev_gpio* clk  = this->cfg.clk_pin;
    struct dev_gpio* mosi = this->cfg.mosi_pin;
    void (*delay)(void) = this->cfg.delay;

    for (uint16_t i = 0; i < len; i++) {
        uint8_t data = tx_buf[i];
        for (int bit = 7; bit >= 0; bit--) {
            // 设置 MOSI
            if (data & (1 << bit))
                mosi->vt->set_up(mosi);
            else
                mosi->vt->set_down(mosi);
            delay();
            // 时钟上升沿
            clk->vt->set_up(clk);
            delay();   // 保持高电平

            // 时钟下降沿
            clk->vt->set_down(clk);
            delay();   // 保持低电平
        }
    }
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

    HAL_SPI_Receive(this->cfg.hspi, rx_buf, len, HAL_MAX_DELAY);
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

    HAL_SPI_TransmitReceive(this->cfg.hspi, tx_buf, rx_buf, len, HAL_MAX_DELAY);
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
    if (!(this->cfg.use_soft_cs)) return;
    this->cfg.cs_pin->vt->set_up(this->cfg.cs_pin);
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
    if (!(this->cfg.use_soft_cs)) return;

    this->cfg.cs_pin->vt->set_down(this->cfg.cs_pin);
}

/*********************************************************************************************************
*   receive data by software
*
*   @param   self    the spi dev
*   @param   rx_buf  receive buffer
*   @param   len     receive length
*   @return  void
*   @note
*********************************************************************************************************/
static void soft_recieve(struct dev_spi* self, uint8_t* rx_buf, uint16_t len)
{
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;
    struct dev_gpio* clk  = this->cfg.clk_pin;
    struct dev_gpio* miso = this->cfg.miso_pin;
    void (*delay)(void) = this->cfg.delay;

    for (uint16_t i = 0; i < len; i++) {
        uint8_t data = 0;
        for (int bit = 7; bit >= 0; bit--) {
            // 时钟上升沿
            clk->vt->set_up(clk);
            delay();   // 保持高电平，等待从设备输出

            // 读取 MISO
            if (miso->vt->read(miso))
                data |= (1 << bit);

            // 时钟下降沿
            clk->vt->set_down(clk);
            delay();   // 保持低电平
        }
        rx_buf[i] = data;
    }
}

/*********************************************************************************************************
*   send and receive data by software
*
*   @param   self    the spi dev
*   @param   tx_buf  send buffer
*   @param   rx_buf  receive buffer
*   @param   len     transfer length
*   @return  void
*   @note
*********************************************************************************************************/
static void soft_transfer(struct dev_spi* self, uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len)
{
    struct dev_spi_impl* this = (struct dev_spi_impl*)self;
    struct dev_gpio* clk  = this->cfg.clk_pin;
    struct dev_gpio* mosi = this->cfg.mosi_pin;
    struct dev_gpio* miso = this->cfg.miso_pin;
    void (*delay)(void) = this->cfg.delay;

    for (uint16_t i = 0; i < len; i++) {
        uint8_t tx_data = tx_buf ? tx_buf[i] : 0xFF;   // 若 tx_buf 为 NULL，发送全 1
        uint8_t rx_data = 0;
        for (int bit = 7; bit >= 0; bit--) {
            // 设置 MOSI
            if (tx_data & (1 << bit))
                mosi->vt->set_up(mosi);
            else
                mosi->vt->set_down(mosi);
            delay();
            // 时钟上升沿
            clk->vt->set_up(clk);
            delay();   // 保持高电平

            // 读取 MISO
            if (miso->vt->read(miso))
                rx_data |= (1 << bit);

            // 时钟下降沿
            clk->vt->set_down(clk);
            delay();   // 保持低电平
        }
        if (rx_buf)
            rx_buf[i] = rx_data;
    }
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

    // 深拷贝配置
    memcpy(&obj->cfg, spi_conf, sizeof(dev_spi_conf));

    // 根据 is_soft_spi 选择虚表
    if (spi_conf->is_soft_spi) {
        obj->dev.vt = &s_soft_spi_vt;
        // 软件 SPI 必须提供 delay 函数
        BSP_Assert(obj->cfg.delay != NULL, "Delay Function is NULL for soft SPI", s_cnt);
    } else {
        obj->dev.vt = &s_spi_vt;
    }

    obj->dev_id = s_cnt - 1;
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
