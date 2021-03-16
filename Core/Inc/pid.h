/*
 * pid.h
 *
 *  Created on: Mar 12, 2021
 *      Author: Gyula
 */

#ifndef INC_PID_H_
#define INC_PID_H_




typedef struct{
	float Kp;
	float Ki;
	float Kd;

	/* LPF for D   */

	float tau;

	/*	Output Limit   */

	float limit_min_output;
	float limit_max_output;


	/*	Ts time	*/

	float T;

	/* Mem block */
	int is_int_winded_up;

	float proportional;
	float integrator;
	float prevIntegrator; // For Anti wind up
	float prevError; // For I
	float differentiator;
	float prevMeasurment;  // for D


	float output;


}PID;


void PID_init(PID *PID_Handler);

float PID_Update(PID *PID_H, float setpoint, float measurment);



#endif /* INC_PID_H_ */
