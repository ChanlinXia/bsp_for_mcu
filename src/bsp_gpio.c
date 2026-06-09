/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
struct dev_gpio
{
    struct dev_gpio_vt* vt;

    uint32_t pin;
    GPIO_TypeDef* port;
};

#ifdef DEV_GPIO_NUM
#ifdef  DEV_LED_NUM
#define GPIO_NUM  DEV_GPIO_NUM + DEV_LED_NUM

#else
#define GPIO_NUM  DEV_GPIO_NUM

#endif

# else
#define GPIO_NUM  1
#endif

static struct dev_gpio s_dev_gpio_list[GPIO_NUM]={};

// static function dec
static void _set_up(struct dev_gpio_vt* self);
static void _set_down(struct dev_gpio_vt* self);
static uint8_t _read(struct dev_gpio_vt* self);
static void _toggle(struct dev_gpio_vt* self);

// 静态虚函数
static struct dev_gpio_vt s_gpio_vt ={
    .set_up = _set_up,
    .set_down = _set_down,
    .read = _read,
    .toggle = _toggle,
};

// static function realization
static void _set_up(struct dev_gpio_vt* self) {
    struct dev_gpio* this = (struct dev_gpio*) self;
    HAL_GPIO_WritePin(this->port,this->pin, GPIO_PIN_SET);
}

static void _set_down(struct dev_gpio_vt* self) {
    struct dev_gpio* this = (struct dev_gpio*) self;
    HAL_GPIO_WritePin(this->port,this->pin, GPIO_PIN_RESET);
}

static uint8_t _read(struct dev_gpio_vt* self) {
    struct dev_gpio* this = (struct dev_gpio*) self;
    GPIO_PinState status=HAL_GPIO_ReadPin(this->port,this->pin);
    if (status == GPIO_PIN_SET) return 1;
    else return 0;
}

static void _toggle(struct dev_gpio_vt* self) {
    struct dev_gpio* this = (struct dev_gpio*) self;
    HAL_GPIO_TogglePin(this->port,this->pin);
}
/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
void GPIO_DevRegister(void* conf) {
    static uint8_t s_cnt=0;

    if (s_cnt >=GPIO_NUM) {
        printf("[GPIO] the index of the register item is out of range");
        return;
    }

    struct dev_gpio* obj = &s_dev_gpio_list[s_cnt++];
    dev_gpio_conf* gpio_conf = (dev_gpio_conf*)conf;

    obj->vt = &s_gpio_vt;
    obj->pin = gpio_conf->pin;
    obj->port = gpio_conf->port;
}

void GPIO_DevGet(struct dev_gpio_vt** obj,uint8_t ind) {
    Assert(ind < GPIO_NUM);

    *obj = (struct dev_gpio_vt*)&s_dev_gpio_list[ind]; // 赋值
}

/* USER CODE END 2 */
