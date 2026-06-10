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

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#ifdef DEV_PWM_NUM
#define PWM_NUM DEV_PWM_NUM
#else
#define PWM_NUM 1
#endif

/*********************************************************************************************************
*                                              Private Declaration
*********************************************************************************************************/
struct dev_pwm
{
    struct dev_pwm_vt* vt;

    TIM_HandleTypeDef* htim;
    uint32_t channel;

    float duty;
    uint32_t freq;
};

/*********************************************************************************************************
*                                              Static Declaration
*********************************************************************************************************/
// static function dec
static void _start(struct dev_pwm_vt* self);
static void _stop(struct dev_pwm_vt* self);

static void _set_duty(struct dev_pwm_vt* self,float duty);
static float _get_duty(struct dev_pwm_vt* self);

static void _set_freq(struct dev_pwm_vt* self,uint32_t hz);

// static device list
static struct dev_pwm s_dev_pwm_list[PWM_NUM]={};

// static virtual function list
static struct dev_pwm_vt s_pwm_vt =
{
    .start = _start,
    .stop = _stop,

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
*   @param   self      the pwm dev
*   @return  void
*   @note
*********************************************************************************************************/
static void _start(struct dev_pwm_vt* self)
{
    struct dev_pwm* this = (struct dev_pwm*)self;

    if(this == NULL || this->htim == NULL)
        return;

    HAL_TIM_PWM_Start(this->htim,this->channel);
}

/*********************************************************************************************************
*   stop pwm output
*
*   @param   self      the pwm dev
*   @return  void
*   @note
*********************************************************************************************************/
static void _stop(struct dev_pwm_vt* self)
{
    struct dev_pwm* this = (struct dev_pwm*)self;

    if(this == NULL || this->htim == NULL)
        return;

    HAL_TIM_PWM_Stop(this->htim,this->channel);
}

/*********************************************************************************************************
*   set pwm duty cycle
*
*   @param   self      the pwm dev
*   @param   duty      0~100(%)
*   @return  void
*   @note
*********************************************************************************************************/
static void _set_duty(struct dev_pwm_vt* self,float duty)
{
    struct dev_pwm* this = (struct dev_pwm*)self;

    if(this == NULL || this->htim == NULL)
        return;

    if(duty < 0.0f)
        duty = 0.0f;

    if(duty > 100.0f)
        duty = 100.0f;

    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(this->htim);

    uint32_t ccr = (uint32_t)((arr + 1) * duty / 100.0f);

    __HAL_TIM_SET_COMPARE(this->htim,
                          this->channel,
                          ccr);

    this->duty = duty;
}

/*********************************************************************************************************
*   get pwm duty cycle
*
*   @param   self      the pwm dev
*   @return  duty(%)
*   @note
*********************************************************************************************************/
static float _get_duty(struct dev_pwm_vt* self)
{
    struct dev_pwm* this = (struct dev_pwm*)self;

    if(this == NULL)
        return 0.0f;

    return this->duty;
}

/*********************************************************************************************************
*   set pwm frequency
*
*   @param   self      the pwm dev
*   @param   hz        target frequency
*   @return  void
*   @note
*********************************************************************************************************/
static void _set_freq(struct dev_pwm_vt* self,uint32_t hz)
{
    struct dev_pwm* this = (struct dev_pwm*)self;

    if(this == NULL || this->htim == NULL)
        return;

    this->freq = hz;

    /*
     * TODO:
     * 根据 APB Timer Clock
     * 重新计算 PSC / ARR
     */
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   init the pwm dev with the conf
*
*   @param   conf      pwm config
*   @return  void
*   @note
*********************************************************************************************************/
void PWM_DevRegister(void* conf)
{
    static uint8_t s_cnt=0;

    Assert(s_cnt < PWM_NUM,"can't register PWM Dev");


    dev_pwm_conf* pwm_conf = (dev_pwm_conf*)conf;

    struct dev_pwm* obj = &s_dev_pwm_list[s_cnt++];

    obj->vt = &s_pwm_vt;

    obj->htim = pwm_conf->htim;
    obj->channel = pwm_conf->channel;

    obj->duty = 0.0f;
    obj->freq = 0;
}

/*********************************************************************************************************
*   get the pwm dev
*
*   @param   obj       pwm object
*   @param   ind       device index
*   @return  void
*   @note
*********************************************************************************************************/
void PWM_DevGet(struct dev_pwm_vt** obj,uint8_t ind)
{
    Assert(ind < PWM_NUM,"can't get PWM Dev");

    *obj = (struct dev_pwm_vt*)&s_dev_pwm_list[ind];
}