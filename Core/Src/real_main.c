/*
 * real_main.c
 *
 *  Created on: Feb 25, 2021
 *      Author: Gyula
 */






#include "main.h"
#include "lcd.h"
#include "bldc_drive.h"
#include "pid.h"

/* LL START */

#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_tim.h"

/* LL END   */

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;



/* Enable/Disable Lcd  */

//#define lcd_enable



/* FUNCTION DECLARE  */

void main_Interrupt();





/* VARIBALE DECLARE  */

int PWM_Period_ARR = 3600;
six_step_handler state_machine_variable_bldc;
adc_result_values adc_last_read_values;
hall_transit_times hall_sensor_transit_times_struct;
PID PID_Handler;







void init_peripheral()
{
	/*    TESTING	START	 */

	set_pwm_duty_non_inverted(&state_machine_variable_bldc, 0);




	/*    TESTING	END	 */


	/*	PID init		*/

	PID_init(&PID_Handler);

	/*  Hall sensor handler init */

	hall_sensor_transit_times_struct.htim = &htim4;


	/* Adc read handler init */

	adc_last_read_values.hadc = &hadc1;


	/*   Six step handler init  */

	state_machine_variable_bldc.actual_direction = 1;
	state_machine_variable_bldc.hall_sensor_Actual_state = 0;
	state_machine_variable_bldc.hall_sensor_Last_state = 0;
	state_machine_variable_bldc.htim = &htim1;
	state_machine_variable_bldc.pwn_period_time_ARR = PWM_Period_ARR;


	/*  TIM 1 PWM START    */

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

	/* Init six step function */

	init_SixStep(state_machine_variable_bldc.htim);

	/*	Start TIM1 for update IT		*/

	HAL_TIM_Base_Start_IT(&htim1);

	/* 	Start ADC  */

	HAL_ADCEx_InjectedStart_IT(&hadc1);

	/*  Start TIM input capture mode  */
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);

}






void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	Adc_read_values(&adc_last_read_values);
	HAL_ADCEx_InjectedStart_IT(&hadc1);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
		main_Interrupt();
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		hall_sensor_transit_times_struct.hall_A_last_transit_time = TIM4->CNT;
		TIM4->CNT = 0;
	}
}



void main_loop()
{



}





void main_Interrupt()
{
	get_mech_period(&hall_sensor_transit_times_struct);
	get_mech_freq(&hall_sensor_transit_times_struct);
	state_machine_variable_bldc.hall_sensor_Actual_state = hall_read_actual_value();
	six_step_commutation(&state_machine_variable_bldc);
}






