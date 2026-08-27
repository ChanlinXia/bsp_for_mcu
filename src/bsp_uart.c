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
#include <stdio.h>

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_UART_NUM
#define UART_NUM  DEV_UART_NUM
#else
#define UART_NUM  1
#endif

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_uart_impl
{
    struct dev_uart dev;              // 公共接口（放在第一个）

    UART_HandleTypeDef* huart;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void send(struct dev_uart* self, uint8_t* tx_buf, uint16_t len);
static void recv(struct dev_uart* self, uint8_t* rx_buf, uint16_t len);

static void send_recv(struct dev_uart* self,
                       uint8_t* tx_buf,
                       uint16_t tx_len,
                       uint8_t* rx_buf,
                       uint16_t rx_len);

// static device list
static struct dev_uart_impl s_dev_uart_list[UART_NUM] = {};

// static virtual function list
static struct dev_uart_vt s_uart_vt = {
    .send      = send,
    .recv      = recv,
    .send_recv = send_recv,
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/
/*********************************************************************************************************
*   send uart data
*
*   @param   self    the uart dev
*   @param   tx_buf  transmit buffer
*   @param   len     transmit length
*   @return  void
*   @note
*********************************************************************************************************/
static void send(struct dev_uart* self, uint8_t* tx_buf, uint16_t len)
{
    struct dev_uart_impl* this = (struct dev_uart_impl*)self;

    if (this == NULL || this->huart == NULL)
        return;

    HAL_UART_Transmit(this->huart, tx_buf, len, HAL_MAX_DELAY);
}

/*********************************************************************************************************
*   receive uart data
*
*   @param   self    the uart dev
*   @param   rx_buf  receive buffer
*   @param   len     receive length
*   @return  void
*   @note
*********************************************************************************************************/
static void recv(struct dev_uart* self, uint8_t* rx_buf, uint16_t len)
{
    struct dev_uart_impl* this = (struct dev_uart_impl*)self;

    if (this == NULL || this->huart == NULL)
        return;

    HAL_UART_Receive(this->huart, rx_buf, len, HAL_MAX_DELAY);
}

/*********************************************************************************************************
*   send then receive uart data
*
*   @param   self    the uart dev
*   @param   tx_buf  transmit buffer
*   @param   tx_len  transmit length
*   @param   rx_buf  receive buffer
*   @param   rx_len  receive length
*   @return  void
*   @note
*********************************************************************************************************/
static void send_recv(struct dev_uart* self,
                       uint8_t* tx_buf,
                       uint16_t tx_len,
                       uint8_t* rx_buf,
                       uint16_t rx_len)
{
    send(self, tx_buf, tx_len);
    recv(self, rx_buf, rx_len);
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   register the uart device
*
*   @param   conf  the uart config
*   @return  void
*   @note
*********************************************************************************************************/
void UART_DevRegister(void* conf)
{
    static uint8_t s_cnt = 0;

    BSP_Assert(s_cnt < UART_NUM, "Fail to register the UART dev",s_cnt);

    struct dev_uart_impl* obj = &s_dev_uart_list[s_cnt++];
    dev_uart_conf* uart_conf = (dev_uart_conf*)conf;

    obj->dev.vt   = &s_uart_vt;
    obj->huart    = uart_conf->huart;
}

/*********************************************************************************************************
*   get the uart device
*
*   @param   obj   the uart device pointer (output)
*   @param   ind   device index
*   @return  void
*   @note
*********************************************************************************************************/
void UART_DevGet(struct dev_uart** obj, uint8_t ind)
{
    BSP_Assert(ind < UART_NUM, "Fail to get the UART dev",ind);

    *obj = (struct dev_uart*)&s_dev_uart_list[ind];
}

/*********************************************************************************************************
*   for printf gcc compiler
*
*   @param   ch    character to send
*   @return  int   character sent
*   @note    Uncomment and implement based on your uart instance
*********************************************************************************************************/
//int __io_putchar(int ch)
//{
//    /* 发送一个字节到 USART1，超时时间根据需要调整 */
//    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
//    return ch;
//}

/*********************************************************************************************************
*   for printf keil compiler
*
*   @param   ch    character to send
*   @return  int   character sent
*   @note    Uncomment and implement based on your uart instance
*********************************************************************************************************/
int fputc(int ch, FILE *f)
{
    /* 发送一个字节到 USART1 */
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
