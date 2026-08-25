/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/6/3.
*   @version  1.0
*   @update
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Header
*********************************************************************************************************/
#include "bsp_pwm.h"
#include <stdio.h>

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_PWM_NUM
#define PWM_NUM  DEV_PWM_NUM
#else
#define PWM_NUM  1
#endif

#define IS_ADVANCE_TIM(ins) ( (ins) == TIM1 || (ins) == TIM8 )

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/

struct dev_pwm_impl
{
    struct dev_pwm dev;           // 公共接口（放在第一个）

    dev_pwm_oc_config oc_config;

    double duty;                  // 当前占空比 (%)
    uint32_t freq;                // 当前频率 (Hz)
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void  enable(struct dev_pwm* self);
static void  disable(struct dev_pwm* self);

static void  set_duty(struct dev_pwm* self, double duty);
static double get_duty(struct dev_pwm* self);
static uint16_t set_freq(struct dev_pwm* self, uint32_t hz,uint32_t min_resolution);
static void  set_duty_regv(struct dev_pwm* self, uint32_t reg_val);
static dev_pwm_oc_config* get_oc_config_ptr(struct dev_pwm* self);
static void set_duty_us(struct dev_pwm* self, uint32_t nus);
static void update_oc_config(struct dev_pwm* self);

static void dev_pwm_sync_oc_config(TIM_HandleTypeDef *htim, uint32_t channel,
                           uint32_t timer_clock, dev_pwm_oc_config *out_cfg);

static void dev_pwm_load_oc_config(const dev_pwm_oc_config *cfg);

uint32_t pwm_calc_prescaler(
    uint32_t timer_clk,
    uint32_t frequency,
    uint32_t arr_max)
{
    if (timer_clk == 0 || frequency == 0)
    {
        return 0;
    }


    uint64_t denominator =
        (uint64_t)frequency *
        (arr_max + 1);


    /*
     * ceil(timer_clk / denominator)
     *
     */
    uint32_t divider =
        (timer_clk + denominator - 1)
        /
        denominator;


    /*
     * divider = PSC + 1
     */
    if (divider == 0)
    {
        divider = 1;
    }


    return divider - 1;
}

// static device list
static struct dev_pwm_impl s_dev_pwm_list[PWM_NUM] = {};

// static virtual function list
static struct dev_pwm_vt s_pwm_vt = {
    .start    = enable,
    .stop     = disable,
    .set_duty_perc = set_duty,
    .set_duty_regv=set_duty_regv,
    .get_duty = get_duty,
    .set_freq = set_freq,
    .get_oc_config_ptr=get_oc_config_ptr,
    .set_duty_us = set_duty_us,
    .update_oc_config=update_oc_config
};

/*********************************************************************************************************
*                                              Static Functions
*********************************************************************************************************/
/*********************************************************************************************************
*   start pwm output
*
*   @param   self  the pwm dev
*   @return  void
*   @note
*********************************************************************************************************/
static void enable(struct dev_pwm* self)
{
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    if (this == NULL || this->oc_config.htim == NULL)
        return;

    HAL_TIM_PWM_Start(this->oc_config.htim, this->oc_config.channel);
}

/*********************************************************************************************************
*   stop pwm output
*
*   @param   self  the pwm dev
*   @return  void
*   @note
*********************************************************************************************************/
static void disable(struct dev_pwm* self)
{
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    if (this == NULL || this->oc_config.htim == NULL)
        return;

    HAL_TIM_PWM_Stop(this->oc_config.htim, this->oc_config.channel);
}

/*********************************************************************************************************
*   set pwm duty cycle
*
*   @param   self  the pwm dev
*   @param   duty  0~100 (%)
*   @return  void
*   @note
*********************************************************************************************************/
static void set_duty(struct dev_pwm* self, double duty)
{
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    if (this == NULL || this->oc_config.htim == NULL)
        return;

    if (duty < 0.0f)
        duty = 0.0f;

    if (duty > 1.0f)
        duty = 1.0f;

    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(this->oc_config.htim);
    uint32_t ccr = (uint32_t)((arr + 1) * duty );

    __HAL_TIM_SET_COMPARE(this->oc_config.htim, this->oc_config.channel, ccr);

    this->duty = duty;
}

/*********************************************************************************************************
*   get pwm duty cycle
*
*   @param   self  the pwm dev
*   @return  duty (%)
*   @note
*********************************************************************************************************/
static double get_duty(struct dev_pwm* self)
{
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    if (this == NULL)
        return 0.0f;

    return this->duty;
}

/*********************************************************************************************************
*   get pwm duty cycle
*
*   @param   self  the pwm dev
*   @return  duty (%)
*   @note
*********************************************************************************************************/
static void set_duty_us(struct dev_pwm* self, uint32_t nus) {
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    uint32_t ccr = time_to_tick(nus,&this->oc_config);

    set_duty_regv(self,ccr);
}

/*********************************************************************************************************
*   set pwm frequency
*
*   @param   self  the pwm dev
*   @param   hz    target frequency (Hz)
*   @return  void
*   @note
*********************************************************************************************************/
static uint16_t set_freq(
    struct dev_pwm* self,
    uint32_t hz,
    uint32_t arr_max)
{
    struct dev_pwm_impl* this =
        (struct dev_pwm_impl*)self;


    if (this == NULL ||
        this->oc_config.htim == NULL ||
        hz == 0)
    {
        return 0;
    }


    uint32_t timer_clk =
        this->oc_config.timer_clock;


    /*
     * Step1:
     * 初始计算 PSC
     */
    uint32_t psc =
        pwm_calc_prescaler(
            timer_clk,
            hz,
            arr_max);



    /*
     * Step2:
     * 计算 ARR
     */
    uint64_t arr =
        (uint64_t)timer_clk /
        ((uint64_t)(psc + 1) * hz);


    if (arr > 0)
    {
        arr -= 1;
    }



    /*
     * Step3:
     * ARR溢出修正
     */
    if (arr > arr_max)
    {
        /*
         * 固定 ARR 最大
         *
         * ARR+1 = arr_max+1
         *
         * PSC+1 =
         * timer_clk /
         * (freq*(ARR+1))
         */


        psc =
            ((uint64_t)timer_clk /
            ((uint64_t)hz *
             (arr_max + 1)))
            - 1;


        arr = arr_max;
    }



    /*
     * Step4:
     * 写寄存器
     */

    __HAL_TIM_SET_PRESCALER(
        this->oc_config.htim,
        psc);


    __HAL_TIM_SET_AUTORELOAD(
        this->oc_config.htim,
        (uint32_t)arr);


    return (uint16_t)arr;
}

/*********************************************************************************************************
*   set pwm frequency
*
*   @param   self  the pwm dev
*   @param   hz    target frequency (Hz)
*   @return  void
*   @note
*********************************************************************************************************/
static void  set_duty_regv(struct dev_pwm* self, uint32_t reg_val){
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;
    __HAL_TIM_SET_COMPARE(this->oc_config.htim, this->oc_config.channel, reg_val);

    this->duty = reg_val * 1.0 / __HAL_TIM_GET_AUTORELOAD(this->oc_config.htim);
}

/*********************************************************************************************************
  * @brief 读取定时器输出比较通道的完整配置
  *
  * @param htim     定时器句柄
  * @param channel  通道 (TIM_CHANNEL_1 ~ 4)
  * @param timer_clock 定时器时钟频率 (Hz)
  * @param out_cfg  输出的配置结构体指针
*********************************************************************************************************/
void dev_pwm_sync_oc_config(TIM_HandleTypeDef *htim, uint32_t channel,
                           uint32_t timer_clock, dev_pwm_oc_config *out_cfg) {
    TIM_TypeDef *tim = htim->Instance;
    uint32_t ccmr_val = 0, ccer_val = 0, cr2_val = 0, bdtr_val = 0;
    uint32_t shift_mode = 0, shift_pe = 0, shift_fe = 0; // 位偏移量

    // ---- 1. 基础赋值 ----
    out_cfg->htim = htim;
    out_cfg->channel = channel;
    out_cfg->timer_clock = timer_clock;
    // out_cfg->dead_time = 0;

    // ---- 2. 根据通道号确定寄存器位偏移 ----
    if (channel == TIM_CHANNEL_1) {
        shift_mode = TIM_CCMR1_OC1M_Pos;   // 通常为 4
        shift_pe   = TIM_CCMR1_OC1PE_Pos;  // 通常为 3
        shift_fe   = TIM_CCMR1_OC1FE_Pos;  // 通常为 2
        ccmr_val = tim->CCMR1;
    } else if (channel == TIM_CHANNEL_2) {
        shift_mode = TIM_CCMR1_OC2M_Pos;   // 通常为 12
        shift_pe   = TIM_CCMR1_OC2PE_Pos;  // 通常为 11
        shift_fe   = TIM_CCMR1_OC2FE_Pos;  // 通常为 10
        ccmr_val = tim->CCMR1;
    } else if (channel == TIM_CHANNEL_3) {
        shift_mode = TIM_CCMR2_OC3M_Pos;   // 通常为 4
        shift_pe   = TIM_CCMR2_OC3PE_Pos;  // 通常为 3
        shift_fe   = TIM_CCMR2_OC3FE_Pos;  // 通常为 2
        ccmr_val = tim->CCMR2;
    } else if (channel == TIM_CHANNEL_4) {
        shift_mode = TIM_CCMR2_OC4M_Pos;   // 通常为 12
        shift_pe   = TIM_CCMR2_OC4PE_Pos;  // 通常为 11
        shift_fe   = TIM_CCMR2_OC4FE_Pos;  // 通常为 10
        ccmr_val = tim->CCMR2;
    } else {
        return; // 无效通道
    }

    // ---- 3. 读取通用寄存器 ----
    ccer_val = tim->CCER;
    cr2_val = tim->CR2;
    bdtr_val = tim->BDTR;  // 高级定时器专用，通用定时器此寄存器为0

    // ---- 4. 提取模式 (Mode) ----
    // OC1M 字段占 3 位 (bit 16~18), 掩码为 0b111
    uint32_t mode_bits = (ccmr_val >> shift_mode) & 0x7;
    out_cfg->mode = mode_bits;  // 可以直接返回数值，也可转为HAL宏
    // 注：数值对应关系：0b000=冻结, 0b001=有效, 0b010=无效, 0b011=翻转,
    // 0b110=PWM1, 0b111=PWM2 (详见STM32参考手册)

    // ---- 5. 提取预装载使能 (Preload) ----
    out_cfg->preload_enable = (ccmr_val >> shift_pe) & 0x1;

    // ---- 6. 提取快速使能 (Fast) ----
    out_cfg->fast_enable = (ccmr_val >> shift_fe) & 0x1;

    // ---- 7. 提取极性 (Polarity) 和 输出使能 (Output Enable) ----
    // 通道1的极性位在 CCER 的 bit1，通道2在 bit5，以此类推 (每通道占4位)
    uint32_t ch_shift = 0;
    if (channel == TIM_CHANNEL_1) ch_shift = 0;
    else if (channel == TIM_CHANNEL_2) ch_shift = 4;
    else if (channel == TIM_CHANNEL_3) ch_shift = 8;
    else if (channel == TIM_CHANNEL_4) ch_shift = 12;

    out_cfg->polarity = ((ccer_val >> (ch_shift + 1)) & 0x1) ? TIM_OCPOLARITY_LOW : TIM_OCPOLARITY_HIGH;
    out_cfg->output_enable = (ccer_val >> (ch_shift)) & 0x1;   // CCxE 位

    // [TODO] 有机会把下面的部分做适配
    if (!IS_ADVANCE_TIM(htim->Instance)) return;

    // ---- 8. 互补通道使能 (仅高级定时器有效) ----
    out_cfg->complement_enable = (ccer_val >> (ch_shift + 2)) & 0x1; // CCxNE 位

    // ---- 9. 提取空闲状态 (OISx / OISxN) ----
    // OISx 位位于 CR2 寄存器，通道1在 bit8，通道2在 bit9，通道3在 bit10，通道4在 bit11
    uint32_t ois_shift = 8 + ((channel >> 2) & 0x3); // 公式: CH1->8, CH2->9, CH3->10, CH4->11
    // out_cfg->idle_state = (cr2_val >> ois_shift) & 0x1;
    // OISxN 位紧跟在后面 (bit + 1)
    // out_cfg->complement_idle = (cr2_val >> (ois_shift + 1)) & 0x1;

    // ---- 10. 提取死区时间 (仅高级定时器) ----
    // DT (Dead Time) 位于 BDTR 寄存器的 bit0~7
    // if (htim->Init.BreakState != TIM_BREAK_DISABLE) { // 粗略判断是否为高级定时器
    //     uint32_t dt_value = bdtr_val & 0xFF;
    //     out_cfg->dead_time = dt_value; // 实际时间需根据 timer_clock 计算，公式见STM32手册
    // }
}

/*********************************************************************************************************
  * @brief 将config装载进寄存器
  *
  * @param cfg     定时器句柄
*********************************************************************************************************/
static void dev_pwm_load_oc_config(const dev_pwm_oc_config *cfg)
{
    TIM_OC_InitTypeDef oc_cfg = {0};


    if(cfg == NULL || cfg->htim == NULL)
    {
        return;
    }


    /*
     * Basic OC configuration
     */
    oc_cfg.OCMode = cfg->mode;

    oc_cfg.OCPolarity = cfg->polarity;

    oc_cfg.OCFastMode =
        cfg->fast_enable ?
        TIM_OCFAST_ENABLE :
        TIM_OCFAST_DISABLE;

    /*
     * Configure PWM channel
     *
     * This will configure:
     * CCMR
     * CCER polarity
     * Output compare mode
     */
    HAL_StatusTypeDef ret = HAL_OK;

    if (cfg->mode == TIM_OCMODE_PWM1 ||
        cfg->mode == TIM_OCMODE_PWM2 ) {
        ret = HAL_TIM_PWM_ConfigChannel(
            cfg->htim,
            &oc_cfg,
            cfg->channel);
    }
    else {
        ret = HAL_TIM_OC_ConfigChannel(
                   cfg->htim,
                   &oc_cfg,
                   cfg->channel);
    }




    if(ret != HAL_OK)
    {
        printf("fail to load pwm config\r\n");
        return;
    }


    /*
     * CCR preload enable
     *
     * HAL TIM_OC_InitTypeDef does not contain preload field.
     *
     * Need configure after HAL.
     */
    if(cfg->preload_enable)
    {
        switch(cfg->channel)
        {
            case TIM_CHANNEL_1:
                cfg->htim->Instance->CCMR1 |= TIM_CCMR1_OC1PE;
                break;


            case TIM_CHANNEL_2:
                cfg->htim->Instance->CCMR1 |= TIM_CCMR1_OC2PE;
                break;


            case TIM_CHANNEL_3:
                cfg->htim->Instance->CCMR2 |= TIM_CCMR2_OC3PE;
                break;


            case TIM_CHANNEL_4:
                cfg->htim->Instance->CCMR2 |= TIM_CCMR2_OC4PE;
                break;


            default:
                break;
        }
    }

    /*
     * Channel output enable
     */
    if(cfg->output_enable == 1)
    {
        HAL_TIM_PWM_Start(cfg->htim,cfg->channel);
    }
    else if (cfg->output_enable == 0)
    {
        HAL_TIM_PWM_Stop(cfg->htim,cfg->channel);
    }
    else {
        HAL_TIM_OC_Stop(cfg->htim,cfg->channel);

    }

    /*
     * Advanced timer complementary output
     *
     */
    // if(IS_ADVANCE_TIM(cfg->htim->Instance))
    // {
    //
    //     /*
    //      * Dead time configuration
    //      *
    //      * cfg->dead_time:
    //      * HAL BDTR.DeadTime value
    //      */
    //     TIM_BreakDeadTimeConfigTypeDef bdtr = {0};
    //
    //
    //     bdtr.DeadTime = cfg->dead_time;
    //
    //
    //     /*
    //      * keep other safety options default.
    //      * pwm_pair will configure them later.
    //      */
    //     bdtr.AutomaticOutput =
    //             TIM_AUTOMATICOUTPUT_ENABLE;
    //
    //
    //     HAL_TIMEx_ConfigBreakDeadTime(
    //             cfg->htim,
    //             &bdtr);
    //
    //
    //     /*
    //      * Enable complementary output
    //      */
    //     if(cfg->complement_enable)
    //     {
    //         HAL_TIMEx_PWMN_Start(
    //                 cfg->htim,
    //                 cfg->channel);
    //     }
    //     else
    //     {
    //         HAL_TIMEx_PWMN_Stop(
    //                 cfg->htim,
    //                 cfg->channel);
    //     }
    //
    // }
}

/*********************************************************************************************************
  * @brief 读取定时器输出比较通道的完整配置
  *
  * @param self     定时器句柄
*********************************************************************************************************/
static dev_pwm_oc_config* get_oc_config_ptr(struct dev_pwm* self) {
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    return &this->oc_config;
}

/*********************************************************************************************************
  * @brief 将现在的config写进寄存器
  *
  * @param self     定时器句柄
  * @return void
*********************************************************************************************************/
static void update_oc_config(struct dev_pwm* self) {
    //[TODO]

}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   register the pwm device
*
*   @param   conf  the pwm config
*   @return  void
*   @note
*********************************************************************************************************/
void PWM_DevRegister(void* conf)
{
    static uint8_t s_cnt = 0;

    BSP_Assert(s_cnt < PWM_NUM, "Fail to register the PWM dev",s_cnt);

    struct dev_pwm_impl* obj = &s_dev_pwm_list[s_cnt++];
    dev_pwm_basic_config* pwm_conf = (dev_pwm_basic_config*)conf;

    obj->dev.vt   = &s_pwm_vt;

    // obj->oc_config   = *pwm_conf;   // 整体拷贝配置
    obj->duty     = 0.0f;
    obj->freq     = 0;

    dev_pwm_sync_oc_config(
        pwm_conf->htim,
        pwm_conf->channel,
        pwm_conf->timer_clock,
        &obj->oc_config
    );
}

/*********************************************************************************************************
*   get the pwm device
*
*   @param   obj   the pwm device pointer (output)
*   @param   ind   device index
*   @return  void
*   @note
*********************************************************************************************************/
void PWM_DevGet(struct dev_pwm** obj, uint8_t ind)
{
    BSP_Assert(ind < PWM_NUM,
        "Fail to get the PWM dev",ind);

    *obj = (struct dev_pwm*)&s_dev_pwm_list[ind];
}

/*********************************************************************************************************
*   get the config pwm device
*
*   @param   obj   the pwm device pointer (output)
*   @return  void
*   @note
*********************************************************************************************************/
void PWM_GetOCConfig(const dev_pwm_basic_config* config,dev_pwm_oc_config* oc_config){

    dev_pwm_sync_oc_config(
        config->htim,
        config->channel,
        config->timer_clock,
        oc_config
    );
}

/*********************************************************************************************************
*   get the config pwm device
*
*   @param   obj   the pwm device pointer (output)
*   @return  void
*   @note
*********************************************************************************************************/
void PWM_LoadOCConfig(const dev_pwm_oc_config* oc_config) {
    dev_pwm_load_oc_config(oc_config);
}