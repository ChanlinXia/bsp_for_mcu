/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Header
*********************************************************************************************************/
#include "bsp_uart.h"

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_UART_NUM
#define UART_NUM DEV_UART_NUM
#else
#define UART_NUM 1
#endif

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_uart
{
    struct dev_uart_vt* vt;

    UART_HandleTypeDef* huart;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void _send(struct dev_uart_vt* self,uint8_t* tx_buf,uint16_t len);
static void _recv(struct dev_uart_vt* self,uint8_t* rx_buf,uint16_t len);

static void _send_recv(struct dev_uart_vt* self,
                       uint8_t* tx_buf,
                       uint16_t tx_len,
                       uint8_t* rx_buf,
                       uint16_t rx_len);

// static device list
static struct dev_uart s_dev_uart_list[UART_NUM]={};

// static virtual function list
static struct dev_uart_vt s_uart_vt =
{
    .send = _send,
    .recv = _recv,
    .send_recv = _send_recv,
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/
/*********************************************************************************************************
*   send uart data
*
*   @param   self      the uart dev
*   @param   tx_buf    transmit buffer
*   @param   len       transmit length
*   @return  void
*   @note
*********************************************************************************************************/
static void _send(struct dev_uart_vt* self,uint8_t* tx_buf,uint16_t len)
{
    struct dev_uart* this = (struct dev_uart*)self;

    if(this == NULL || this->huart == NULL)
        return;

    HAL_UART_Transmit(this->huart,
                      tx_buf,
                      len,
                      HAL_MAX_DELAY);
}

/*********************************************************************************************************
*   receive uart data
*
*   @param   self      the uart dev
*   @param   rx_buf    receive buffer
*   @param   len       receive length
*   @return  void
*   @note
*********************************************************************************************************/
static void _recv(struct dev_uart_vt* self,uint8_t* rx_buf,uint16_t len)
{
    struct dev_uart* this = (struct dev_uart*)self;

    if(this == NULL || this->huart == NULL)
        return;

    HAL_UART_Receive(this->huart,
                     rx_buf,
                     len,
                     HAL_MAX_DELAY);
}

/*********************************************************************************************************
*   send then receive uart data
*
*   @param   self      the uart dev
*   @param   tx_buf    transmit buffer
*   @param   tx_len    transmit length
*   @param   rx_buf    receive buffer
*   @param   rx_len    receive length
*   @return  void
*   @note
*********************************************************************************************************/
static void _send_recv(struct dev_uart_vt* self,
                       uint8_t* tx_buf,
                       uint16_t tx_len,
                       uint8_t* rx_buf,
                       uint16_t rx_len)
{
    _send(self,tx_buf,tx_len);
    _recv(self,rx_buf,rx_len);
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   init the uart dev with the conf
*
*   @param   conf      uart config
*   @return  void
*   @note
*********************************************************************************************************/
void UART_DevRegister(void* conf)
{
    static uint8_t s_cnt=0;

    Assert(s_cnt < UART_NUM,"can't get UART Dev");


    dev_uart_conf* uart_conf=(dev_uart_conf*)conf;

    struct dev_uart* obj=&s_dev_uart_list[s_cnt++];

    obj->vt=&s_uart_vt;
    obj->huart=uart_conf->huart;
}

/*********************************************************************************************************
*   get the uart dev
*
*   @param   obj       uart object
*   @param   ind       device index
*   @return  void
*   @note
*********************************************************************************************************/
void UART_DevGet(struct dev_uart_vt** obj,uint8_t ind)
{
    Assert(ind < UART_NUM,"can't get UART Dev");

    *obj=(struct dev_uart_vt*)&s_dev_uart_list[ind];
}