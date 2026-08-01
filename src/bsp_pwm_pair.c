/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/7/28.
*   @version  1.0
*   @update   
*********************************************************************************************************/
/*********************************************************************************************************
*                                              Header File
*********************************************************************************************************/
#include "bsp_pwm_pair.h"
#include "bsp_conf.h"

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
#if defined( DEV_PWM_PAIR_NUM)
	#define PWM_PAIR_NUM DEV_PWM_PAIR_NUM
#else
	#define PWM_PAIR_NUM 0
#endif

// #define MAX_DUTY 0.95f

// #define DMA_SW(hdma,sw) do{\
// 	if(!(sw))	HAL_DMA_Abort(hdma);\
// 	else HAL_DMA_Start(hdma,);\
// }while(0)

typedef struct dev_pwm_pair_impl
{
	struct dev_pwm_pair dev;           // 公共接口（放在第一个）

	dev_pwm_pair_conf config;

	dev_pwm_oc_config* pwm_h_conf;
	dev_pwm_oc_config* pwm_l_conf;

	uint32_t oc_reg_dma_buf[2];
	double cur_duty;
}dev_pwm_pair_impl;

/*********************************************************************************************************
*                                              Static Declaration and Variable
*********************************************************************************************************/

static dev_pwm_pair_impl s_pwm_pairs[PWM_PAIR_NUM]={};

/*********************************************************************************************************
*                                              Static Function
*********************************************************************************************************/
/*
 * Declaration
 */
// vt function
static void  start(struct dev_pwm_pair* self);
static void  stop(struct dev_pwm_pair* self);
static void  set_dead_time_us(struct dev_pwm_pair* self, uint16_t new_time);
static double get_duty(struct dev_pwm_pair* self);
static void  set_duty(struct dev_pwm_pair* self,double duty);
static void  set_freq(struct dev_pwm_pair* self, uint32_t hz);

//
static void pwm_default_set(dev_pwm_pair_impl* self);
static uint8_t is_pair_oc_reg_valid(dev_pwm_pair_impl* self);
static void dma_sw(dev_pwm_pair_impl* self,uint8_t sw);
static uint32_t time_to_tick(uint32_t time,const dev_pwm_oc_config* oc_config);
static uint32_t tick_to_time(uint32_t tick,const dev_pwm_oc_config* htim);


static const struct dev_pwm_pair_vt s_pwm_pair_vts={
	.start=start,
	.stop=stop,
	.set_dead_time=set_dead_time_us,
	.get_duty = get_duty,
	.set_duty = set_duty,
	.set_freq = set_freq,
};

/*`
 * Defination
 */
/*********************************************************************************************************
*   switch the dma in case write wrong reg data
*
*   @param   self  the gpio dev
*   @param   sw	   bool
*   @return  void
*   @note
*********************************************************************************************************/
static void dma_sw(dev_pwm_pair_impl* self,uint8_t sw) {

	uint32_t dst_addr_base =  // [TODO] pwm_h对应通道不一定是基地址，且不一定连续，该如何处理？
		__HAL_TIM_GET_COMPARE(self->pwm_h_conf->htim,self->pwm_h_conf->channel);

	if (sw) { // open
		HAL_DMA_Start(self->config.hdma,(uint32_t)self->oc_reg_dma_buf,dst_addr_base,2);
	}
	else {
		HAL_DMA_Abort(self->config.hdma);
	}
}

/*********************************************************************************************************
*   read register and check if the setting is valid
*
*   @param   self  the gpio dev
*   @param   sw	   bool
*   @return  void
*   @note
*********************************************************************************************************/
static uint8_t is_pair_oc_reg_valid(dev_pwm_pair_impl* self){
	return 1;
}

/*********************************************************************************************************
*   start the pair
*
*   @param   self  the pwm_pair dev
*   @return  void
*   @note
*********************************************************************************************************/
static void  start(struct dev_pwm_pair* self) {
	dev_pwm_pair_impl* this = (dev_pwm_pair_impl*)self;
	BSP_Assert(is_pair_oc_reg_valid(this) == 1, // 针对桥式电路直接做寄存器层面的判断
		"[PWMPair] The reg val of OC is not valid",-1);

	this->config.pwm_h->vt->start(this->config.pwm_h);
	this->config.pwm_l->vt->start(this->config.pwm_l);
	dma_sw(this,1);
}

/*********************************************************************************************************
*   stop the pair
*
*   @param   self  the pwm_pair dev
*   @return  void
*   @note
*********************************************************************************************************/
static void  stop(struct dev_pwm_pair* self) {
	dev_pwm_pair_impl* this = (dev_pwm_pair_impl*)self;

	this->config.pwm_h->vt->stop(this->config.pwm_h);
	this->config.pwm_l->vt->stop(this->config.pwm_l);
	dma_sw(this,0);
}

/*********************************************************************************************************
*   set the dead_time
*
*   @param   self   the pwm_pair dev
*   @param new_time the new value of deadtime [us], timer tick <= 1us
*   @return  void
*   @note
*********************************************************************************************************/
static void  set_dead_time_us(struct dev_pwm_pair* self, uint16_t new_time) {
	dev_pwm_pair_impl* this = (dev_pwm_pair_impl*)self;

	this->config.dead_time_tick=time_to_tick(new_time,this->pwm_h_conf);

	set_duty(self,this->cur_duty);
}

/*********************************************************************************************************
*   get the duty
*
*   @param   self   the pwm_pair dev
*   @return  void
*   @note
*********************************************************************************************************/
static double get_duty(struct dev_pwm_pair* self) {
	dev_pwm_pair_impl* this = (dev_pwm_pair_impl*)self;

	return this->cur_duty;
}

/*********************************************************************************************************
*   set the duty
*
*   @param   self   the pwm_pair dev
*   @return  void
*   @note
*********************************************************************************************************/
static void  set_duty(struct dev_pwm_pair* self,double target_duty) {
	dev_pwm_pair_impl* this = (dev_pwm_pair_impl*)self;

	uint32_t dead = this->config.dead_time_tick;
	double safe_duty = target_duty;

	uint32_t arr = __HAL_TIM_GET_AUTORELOAD(this->pwm_h_conf->htim);

	double dead_ratio = 0.5;
	if (arr != 0 ) dead_ratio = (double)(dead * 1.0 / arr);

	// 钳位
	if (target_duty < dead_ratio) safe_duty = dead_ratio;
	if (target_duty > (1-dead_ratio)) safe_duty = 1.0-dead_ratio;

	uint32_t ccr_h = (uint32_t)(safe_duty * arr);

	// 钳位
	if(ccr_h > arr-dead)	ccr_h = arr-dead;
	if(ccr_h < dead)	ccr_h = dead;

	uint32_t ccr_l;

	if(ccr_h + dead >= arr) ccr_l = 0;
	else ccr_l = ccr_h+dead;

	if (this->config.dma_enabled) {	// 如果可用DMA
		dma_sw(this,0);

		this->oc_reg_dma_buf[0]=ccr_h;
		this->oc_reg_dma_buf[1]=ccr_l;

		dma_sw(this,1);
	}
	else { // 否则，只能软件写入
		this->config.pwm_h->vt->set_duty_regv(this->config.pwm_h,ccr_h);
		this->config.pwm_l->vt->set_duty_regv(this->config.pwm_l,ccr_l);
	}

	this->cur_duty = safe_duty;
}

/*********************************************************************************************************
*   set the freq
*
*   @param   self   the pwm_pair dev
*   @return  void
*   @note
*********************************************************************************************************/
static void  set_freq(struct dev_pwm_pair* self, uint32_t hz) {
	dev_pwm_pair_impl* this = (dev_pwm_pair_impl*)self;

	int auto_reload = this->config.pwm_h->vt->set_freq(this->config.pwm_h,hz);
	this->config.pwm_l->vt->set_freq(this->config.pwm_l,hz);

	// update duty value
	set_duty(self,this->config.pwm_h->vt->get_duty(this->config.pwm_h));
	set_duty(self,this->config.pwm_h->vt->get_duty(this->config.pwm_l));

	// 开启DMA
	dma_sw(this,1);
}

/*********************************************************************************************************
*   set the freq
*
*   @param   self   the pwm_pair dev
*   @return  void
*   @note
*********************************************************************************************************/
static uint32_t time_to_tick(uint32_t time,const dev_pwm_oc_config* oc_config)
{
	uint32_t timer_clk;
	uint32_t psc;

	/*
	 * get timer clock
	 */
	timer_clk = oc_config->timer_clock;

	/*
	 * timer prescaler
	 */
	psc = oc_config->htim->Instance->PSC;


	uint64_t cnt_freq;

	cnt_freq = timer_clk / (psc+1);


	/*
	 * us -> tick
	 */
	return (uint32_t)
		(((uint64_t)time * cnt_freq) / 1000000);
}

/*********************************************************************************************************
*   set the freq
*
*   @param   self   the pwm_pair dev
*   @return  void
*   @note
*********************************************************************************************************/
static uint32_t tick_to_time(uint32_t tick,const dev_pwm_oc_config* oc_config){
	uint32_t timer_clk;
	uint32_t psc;

	timer_clk = oc_config->timer_clock;

	psc = oc_config->htim->Instance->PSC;

	uint64_t cnt_freq;
	cnt_freq = timer_clk/(psc+1);

	return (uint32_t)
		(((uint64_t)tick * 1000000)
		/ cnt_freq);
}

/*********************************************************************************************************
*   default setting
*
*   @param   self  the gpio dev
*   @return  void
*   @note
*********************************************************************************************************/
static void pwm_default_set(dev_pwm_pair_impl* self)
{
	dev_pwm_oc_config* h = self->pwm_h_conf;
	dev_pwm_oc_config* l = self->pwm_l_conf;


	/*
	 * H bridge PWM config
	 */

	h->mode = TIM_OCMODE_PWM1;

	h->polarity = TIM_OCPOLARITY_HIGH;

	h->output_enable = 1;

	h->preload_enable = 1;

	h->fast_enable = 0;


	/*
	 * L bridge PWM config
	 */

	l->mode = TIM_OCMODE_PWM1;

	l->polarity = TIM_OCPOLARITY_LOW;

	l->output_enable = 1;

	l->preload_enable = 1;

	l->fast_enable = 0;


	/*
	 * software complementary mode
	 *
	 * not using TIM CHxN
	 */
	h->complement_enable = 0;
	l->complement_enable = 0;



	/*
	 * Load hardware configuration
	 */
	PWM_LoadOCConfig(h);
	PWM_LoadOCConfig(l);

	/*
	 * default runtime parameter
	 */

	self->cur_duty = 0.5;


	set_dead_time_us(
		(struct dev_pwm_pair*)self,
		5
	);


	set_freq(
		(struct dev_pwm_pair*)self,
		1000
	);


	set_duty(
		(struct dev_pwm_pair*)self,
		0.5
	);
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/
/*********************************************************************************************************
*   pull up the pin
*
*   @param   self  the gpio dev
*   @return  void
*   @note   
*********************************************************************************************************/
void PWMPair_DevRegister(void* conf) {
	static uint8_t dev_ind=0;
	dev_pwm_pair_conf* config = conf;

	BSP_Assert(config != NULL,"[PWMPair] The conf is null",dev_ind);

	BSP_Assert(dev_ind < DEV_PWM_PAIR_NUM,
		"[PWMPair] The index is out of range",dev_ind);

	dev_pwm_oc_config* oc_conf_h = config->pwm_h->vt->get_oc_config_ptr(config->pwm_h);
	dev_pwm_oc_config* oc_conf_l = config->pwm_l->vt->get_oc_config_ptr(config->pwm_l);

	// 必须是同一个Timer下的不同channel
	BSP_Assert(oc_conf_h->htim->Instance == oc_conf_l->htim->Instance,
		"[PWMPair] The two in PWMPair have different instance",dev_ind);

	BSP_Assert(oc_conf_h->channel != oc_conf_l->channel,
		"[PWMPair] The two in PWMPair have the same channel",dev_ind);

	BSP_Assert(oc_conf_h->timer_clock == oc_conf_l->timer_clock,
		"[PWMPair] The two in PWMPair have different clock",dev_ind);

	dev_pwm_pair_impl* this = &s_pwm_pairs[dev_ind++];

	this->config.pwm_h = config->pwm_h;
	this->config.pwm_l = config->pwm_l;
	this->config.dead_time_tick = config->dead_time_tick;
	this->config.dma_enabled = config->dma_enabled;
	this->config.hdma = config->hdma;

	this->pwm_h_conf = oc_conf_h;
	this->pwm_l_conf = oc_conf_l;

	this->dev.vt = &s_pwm_pair_vts;

	// 默认设置
	pwm_default_set(this);
}

/*********************************************************************************************************
*   pull up the pin
*
*   @param   self  the gpio dev
*   @return  void
*   @note
*********************************************************************************************************/
void PWMPair_DevGet(struct dev_pwm_pair** obj, uint8_t ind) {
	BSP_Assert(ind < DEV_PWM_PAIR_NUM,
		"[PWMPair] The index is out of range",ind);

	*obj = (struct dev_pwm_pair*)&s_pwm_pairs[ind];
}