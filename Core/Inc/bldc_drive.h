/*
 * bldc_drive.h
 *
 *  Created on: Feb 26, 2021
 *      Author: Gyula
 */
#include "main.h"
#include "real_main.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_tim.h"

#ifndef INC_BLDC_DRIVE_H_
#define INC_BLDC_DRIVE_H_

#define forward_direction 1
#define backward_direction 0

typedef struct{
	int16_t Current_main;
	int16_t Current_V;
	int16_t Current_W;
	int16_t Input_potmeter_value;
	ADC_HandleTypeDef *hadc;
}adc_result_values;


typedef struct{
	int hall_sensor_Actual_state;
	int hall_sensor_Last_state;
	int actual_direction;
	TIM_HandleTypeDef* htim;
	uint16_t pwm_duty;
	uint16_t pwm_duty_normal;
	uint16_t last_pwm_duty;
	uint16_t pwn_period_time_ARR;
}six_step_handler;


typedef struct{
	uint16_t hall_A_last_transit_time;
	float last_mech_freq;
	float last_mech_period_time;
	TIM_HandleTypeDef* htim;
}hall_transit_times;

void set_pwm_duty_non_inverted(six_step_handler *six_step_handler, uint16_t duty);

void get_mech_freq(hall_transit_times *hall_sensor);

void get_mech_period(hall_transit_times *hall_sensor);

int hall_read_actual_value();


void Adc_read_values(adc_result_values *adc);


void init_SixStep(TIM_HandleTypeDef* htim);


void six_step_commutation(six_step_handler *six_step_handler_struct);


#endif /* INC_BLDC_DRIVE_H_ */
