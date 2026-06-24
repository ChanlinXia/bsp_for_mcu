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

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_pwm_impl
{
    struct dev_pwm dev;           // 公共接口（放在第一个）

    TIM_HandleTypeDef* htim;
    uint32_t           channel;

    float  duty;                  // 当前占空比 (%)
    uint32_t freq;                // 当前频率 (Hz)
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void _enable(struct dev_pwm* self);
static void _disable(struct dev_pwm* self);

static void _set_duty(struct dev_pwm* self, float duty);
static float _get_duty(struct dev_pwm* self);

static void _set_freq(struct dev_pwm* self, uint32_t hz);

// static device list
static struct dev_pwm_impl s_dev_pwm_list[PWM_NUM] = {};

// static virtual function list
static struct dev_pwm_vt s_pwm_vt = {
    .start    = _enable,
    .stop     = _disable,
    .set_duty = _set_duty,
    .get_duty = _get_duty,
    .set_freq = _set_freq,
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
static void _enable(struct dev_pwm* self)
{
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    if (this == NULL || this->htim == NULL)
        return;

    HAL_TIM_PWM_Start(this->htim, this->channel);
}

/*********************************************************************************************************
*   stop pwm output
*
*   @param   self  the pwm dev
*   @return  void
*   @note
*********************************************************************************************************/
static void _disable(struct dev_pwm* self)
{
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    if (this == NULL || this->htim == NULL)
        return;

    HAL_TIM_PWM_Stop(this->htim, this->channel);
}

/*********************************************************************************************************
*   set pwm duty cycle
*
*   @param   self  the pwm dev
*   @param   duty  0~100 (%)
*   @return  void
*   @note
*********************************************************************************************************/
static void _set_duty(struct dev_pwm* self, float duty)
{
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    if (this == NULL || this->htim == NULL)
        return;

    if (duty < 0.0f)
        duty = 0.0f;

    if (duty > 100.0f)
        duty = 100.0f;

    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(this->htim);
    uint32_t ccr = (uint32_t)((arr + 1) * duty / 100.0f);

    __HAL_TIM_SET_COMPARE(this->htim, this->channel, ccr);

    this->duty = duty;
}

/*********************************************************************************************************
*   get pwm duty cycle
*
*   @param   self  the pwm dev
*   @return  duty (%)
*   @note
*********************************************************************************************************/
static float _get_duty(struct dev_pwm* self)
{
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    if (this == NULL)
        return 0.0f;

    return this->duty;
}

/*********************************************************************************************************
*   set pwm frequency
*
*   @param   self  the pwm dev
*   @param   hz    target frequency (Hz)
*   @return  void
*   @note   TODO: 需要根据 APB Timer Clock 重新计算 PSC / ARR
*********************************************************************************************************/
static void _set_freq(struct dev_pwm* self, uint32_t hz)
{
    struct dev_pwm_impl* this = (struct dev_pwm_impl*)self;

    if (this == NULL || this->htim == NULL)
        return;

    this->freq = hz;

    /*
     * TODO:
     * 根据 APB Timer Clock 重新计算 PSC / ARR
     * 需要获取定时器的输入时钟频率 (TIMxCLK)
     * 然后计算: ARR+1 = TIMxCLK / (PSC+1) / hz
     */
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

    BSP_Assert(s_cnt < PWM_NUM, "Fail to register the PWM dev");

    struct dev_pwm_impl* obj = &s_dev_pwm_list[s_cnt++];
    dev_pwm_conf* pwm_conf = (dev_pwm_conf*)conf;

    obj->dev.vt   = &s_pwm_vt;
    obj->htim     = pwm_conf->htim;
    obj->channel  = pwm_conf->channel;
    obj->duty     = 0.0f;
    obj->freq     = 0;
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
    BSP_Assert(ind < PWM_NUM, "Fail to get the PWM dev");

    *obj = (struct dev_pwm*)&s_dev_pwm_list[ind];
}