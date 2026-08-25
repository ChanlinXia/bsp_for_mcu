/*********************************************************************************************************
*   This device is designed for oc(output compare) and pwm.
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update
*********************************************************************************************************/
#ifndef BSP_PWM_H
#define BSP_PWM_H

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
#define MAX_ARR 0XFFFF

/*********************************************************************************************************
*                                               Public Declaration
*********************************************************************************************************/
struct dev_pwm;
struct dev_pwm_oc_config;

struct dev_pwm_vt
{
    void   (*start)(struct dev_pwm* self);
    void   (*stop)(struct dev_pwm* self);

    void   (*set_duty_perc)(struct dev_pwm* self, double duty);
    void   (*set_duty_regv)(struct dev_pwm* self, uint32_t reg_val);
    void   (*set_duty_us)(struct dev_pwm* self, uint32_t nus);
    double (*get_duty)(struct dev_pwm* self);

    uint16_t  (*set_freq)(struct dev_pwm* self, uint32_t hz,uint32_t min_resolution);

    struct dev_pwm_oc_config* (*get_oc_config_ptr)(struct dev_pwm* self);

    void (*update_oc_config)(struct dev_pwm* self);
};

struct dev_pwm
{
    struct dev_pwm_vt* vt;
};

typedef struct dev_pwm_basic_config // 初始化
{
    TIM_HandleTypeDef* htim;
    uint32_t           channel;
    uint32_t           timer_clock;

} dev_pwm_basic_config;

typedef struct dev_pwm_oc_config { // oc参数
    /*
     * Device Mapping
     */
    TIM_HandleTypeDef *htim;
    uint32_t channel;

    /*
     * Generic Setting
     */
    uint32_t mode;                // 输出比较/PWM模式 (如 TIM_OCMODE_PWM1)
    uint32_t polarity;            // 输出极性 (TIM_OCPOLARITY_HIGH/LOW)
    uint8_t  output_enable;       // 通道输出使能 (1: 使能, 0: 禁止) —— 极其重要！
    uint8_t  preload_enable;      // CCR预装载使能 (1: 使能, 0: 立即更新) —— 决定时序！
    uint8_t  fast_enable;         // 快速使能 (高级功能，通常为0)

    /*
     * Complementary
     */
    uint8_t  complement_enable;   // 互补通道输出使能 (CCxNE)
    uint32_t dead_time;           // 死区时间 (从 BDTR 寄存器读取，单位: 定时器时钟周期)

    // 下面应该由pwm_pair管理
    // uint8_t  idle_state;          // 空闲状态 (OISx, 仅在高级定时器有效)
    // uint8_t  complement_idle;     // 互补空闲状态 (OISxN)

    /*
     * Runtime
     */
    uint32_t timer_clock;     // 定时器在时钟树上的频率，初始化后不应该修改

}dev_pwm_oc_config;

/*********************************************************************************************************
*                                               API
*********************************************************************************************************/
void PWM_DevRegister(void* conf);
void PWM_DevGet(struct dev_pwm** obj, uint8_t ind);

void PWM_GetOCConfig(const dev_pwm_basic_config* config,dev_pwm_oc_config* oc_config);
void PWM_LoadOCConfig(const dev_pwm_oc_config* oc_config) ;


static inline uint32_t time_to_tick(
    uint32_t time_us,
    const dev_pwm_oc_config* oc_config)
{
    uint64_t numerator;
    uint64_t denominator;

    uint32_t psc = oc_config->htim->Instance->PSC;

    /*
     * tick =
     *
     * time_us * timer_clk
     * --------------------
     * (PSC+1)*1e6
     *
     */

    numerator =
        (uint64_t)time_us *
        oc_config->timer_clock;


    denominator =
        ((uint64_t)psc + 1) *
        1000000ULL;


    // round
    return (uint32_t)
        ((numerator + denominator / 2)
        / denominator);
}

// static inline uint32_t time_to_tick_float(
//     uint32_t time,
//     const dev_pwm_oc_config* oc_config) {
//     double timer_clk = (double)oc_config->timer_clock;
//     double psc = (double)oc_config->htim->Instance->PSC;
//
//     // tick = time (us) * timer_clk (Hz) / ((psc+1) * 1e6)
//     double tick = ((double)time) * timer_clk / ((psc + 1.0) * 1000000.0);
//
//     // 四舍五入到最近的整数
//     return (uint32_t)(tick+0.5);
// }


static inline uint32_t tick_to_time(
    uint32_t tick,
    const dev_pwm_oc_config* oc_config)
{
    uint64_t timer_clk;
    uint32_t psc;


    timer_clk = oc_config->timer_clock;
    psc = oc_config->htim->Instance->PSC;


    return (uint32_t)(
        ((uint64_t)tick
        * (psc + 1)
        * 1000000)
        /
        timer_clk
    );
}

static inline uint32_t tick_to_time_float(
    uint32_t tick,
    const dev_pwm_oc_config* oc_config)
{
    uint64_t timer_clk;
    uint32_t psc;


    timer_clk = oc_config->timer_clock;
    psc = oc_config->htim->Instance->PSC;


    return (uint32_t)(
        ((uint64_t)tick
        * (psc + 1)
        * 1000000)
        /
        timer_clk
    );
}


#ifdef __cplusplus
}
#endif

#endif /* BSP_PWM_H */
