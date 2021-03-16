/*
 * bldc_drive.c
 *
 *  Created on: Feb 26, 2021
 *      Author: Gyula
 */
#include "main.h"
#include "bldc_drive.h"



#define enable_pwm 0x7 // 0b111  CCMRx->OCxM PWM MODE 2
#define force_active 0x5 // 0b100  CCMRx->OCxM Force inactive, in PWM mode 2 it is inverse of OCREF so active mean disable
#define force_inactive 0x4 // 0b101 CCMRx->OCxM Force active, in PWM mode 2 it is inverse of OCREF so inactive mean enable

/* 		TIMx_CCER registers		   */

#define timreg_channel_1_Normal_Enable (1<<0)
#define timreg_channel_1_Normal_Polarity (1<<1)
#define	timreg_channel_1_Complementary_Enable (1<<2)
#define timreg_channel_1_Complementary_Polarity (1<<3)
#define timreg_channel_2_Normal_Enable (1<<4)
#define timreg_channel_2_Normal_Polarity (1<<5)
#define	timreg_channel_2_Complementary_Enable (1<<6)
#define timreg_channel_2_Complementary_Polarity (1<<7)
#define timreg_channel_3_Normal_Enable (1<<8)
#define timreg_channel_3_Normal_Polarity (1<<9)
#define	timreg_channel_3_Complementary_Enable (1<<10)
#define timreg_channel_3_Complementary_Polarity (1<<11)


//CCxP=0 (OCx active high) => OCx is forced to high level
// In PWM mode 2 OUTPUT = INVERSE ( OCREF ) => Force high = High side low, low side high, so Polarity Change is needed!
// LOW side basically active high.





int hall_CCER_reg[8] = {
		0,
		timreg_channel_1_Normal_Enable + timreg_channel_2_Complementary_Enable, //AB
		timreg_channel_2_Normal_Enable + timreg_channel_3_Complementary_Enable, //BC
		timreg_channel_1_Normal_Enable + timreg_channel_3_Complementary_Enable, //AC
		timreg_channel_3_Normal_Enable + timreg_channel_1_Complementary_Enable, //CA
		timreg_channel_3_Normal_Enable + timreg_channel_2_Complementary_Enable, //CB
		timreg_channel_2_Normal_Enable + timreg_channel_1_Complementary_Enable, //BA
		0

};


/* Init timers in choosen pwm mode  */

void init_SixStep(TIM_HandleTypeDef* htim)
{
	htim->Instance->CCMR1 &= (~TIM_CCMR1_OC1M_Msk) + (~TIM_CCMR1_OC2M_Msk);
	htim->Instance->CCMR2 &= (~TIM_CCMR2_OC3M_Msk) + (~TIM_CCMR2_OC4M_Msk);
	htim->Instance->CCMR1 |= ((enable_pwm << TIM_CCMR1_OC1M_Pos) + (enable_pwm << TIM_CCMR1_OC2M_Pos));
	htim->Instance->CCMR2 |= enable_pwm << TIM_CCMR2_OC3M_Pos;
}


void six_step_commutation(six_step_handler *handler)
{
	int saved_hall_state_actual = (handler->hall_sensor_Actual_state & 0x7);

	uint16_t duty_cycle = ((handler->pwm_duty * handler->pwn_period_time_ARR) >> 16); // 16BIT PWM

	if((handler->hall_sensor_Actual_state) != (handler->hall_sensor_Last_state))
	{
		handler->htim->Instance->CCER = hall_CCER_reg[saved_hall_state_actual];
	};

	if(handler->pwm_duty != handler->last_pwm_duty)
	{
		handler->htim->Instance->CCR1 = duty_cycle;
		handler->htim->Instance->CCR2 = duty_cycle;
		handler->htim->Instance->CCR3 = duty_cycle;
	}
	handler->hall_sensor_Last_state = saved_hall_state_actual;

}

int hall_read_actual_value()
{
	int hall_status = ~(((GPIOB->IDR & ((1 << 6)  | (1 << 7  ) |(1 << 8  ) ))) >> 6) & 0x7;
	return hall_status;
}

void Adc_read_values(adc_result_values *adc)
{
	adc->Current_main = ((adc->hadc->Instance->JDR1) >> 4) - 0x7F;
	adc->Current_V = ((adc->hadc->Instance->JDR2) >> 4) - 0x7F;
	adc->Current_W = ((adc->hadc->Instance->JDR3) >> 4) - 0x7F;
	adc->Input_potmeter_value = (adc->hadc->Instance->JDR4);
}

/*FREQ CALC X * (ARR/T)/5			*/

void get_mech_freq(hall_transit_times *hall_sensor)
{
	hall_sensor->last_mech_freq = ((float) 21262 / hall_sensor->hall_A_last_transit_time);
}


/*PERIOD CALC: X * T/ARR *5   */

void get_mech_period(hall_transit_times *hall_sensor)
{
	hall_sensor->last_mech_period_time = ((float) hall_sensor->hall_A_last_transit_time) * ((float) 0.000047031);

}


void set_pwm_duty_non_inverted(six_step_handler *six_step_handler, uint16_t duty)
{
	six_step_handler->pwm_duty_normal = duty;
	six_step_handler->pwm_duty = 65535 - duty;
}
