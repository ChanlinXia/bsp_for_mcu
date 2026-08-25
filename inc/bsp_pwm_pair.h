/*********************************************************************************************************
*	the device is designed to generate complementary pwm [TODO] how to solve the dead time in the end of a period
*
*   @author   Created by Chanlin on 2026/7/28.
*   @version  1.0
*   @update   
*********************************************************************************************************/

#ifndef BSP_PWM_PAIR_H
#define BSP_PWM_PAIR_H

/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include "bsp_conf.h"
#include "bsp_pwm.h"

/*********************************************************************************************************
*                                               Public Macro
*********************************************************************************************************/

/*********************************************************************************************************
*                                               Enum
*********************************************************************************************************/

/*********************************************************************************************************
*                                               Public Structure
*********************************************************************************************************/
struct dev_pwm_pair;

struct dev_pwm_pair_vt
{
	void   (*start)(struct dev_pwm_pair* self);
	void   (*stop)(struct dev_pwm_pair* self);

	void   (*set_dead_time)(struct dev_pwm_pair* self, uint16_t new_time);

	double (*get_duty)(struct dev_pwm_pair* self);
	void  (*set_duty)(struct dev_pwm_pair* self,double duty);

	void   (*set_freq)(struct dev_pwm_pair* self, uint32_t hz);
};

struct dev_pwm_pair
{
	struct dev_pwm_pair_vt* vt;
};

typedef struct
{
	/*
	 * Channel
	 */
	struct dev_pwm* pwm_h;
	struct dev_pwm* pwm_l;

	/*
	 * dead_time
	 */
	 uint32_t dead_time_tick;

	/*
	 * update by dma
	 */
	uint8_t dma_enabled;
	DMA_HandleTypeDef* hdma;

	/*
	 * safety ctrl
	 */
	uint8_t break_enable;
	uint8_t idle_high;
	uint8_t idle_low;
} dev_pwm_pair_conf;

/*********************************************************************************************************
*                                               API List
*********************************************************************************************************/
void PWMPair_DevRegister(void* conf);
void PWMPair_DevGet(struct dev_pwm_pair** obj, uint8_t ind);

#endif //BSP_PWM_PAIR_H
