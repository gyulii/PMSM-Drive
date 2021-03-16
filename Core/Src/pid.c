/*
 * pid.c
 *
 *  Created on: Mar 12, 2021
 *      Author: Gyula
 */

#include "pid.h"


void PID_init(PID *PID_Handler)
{
	PID_Handler->integrator = 0;
	PID_Handler->prevError = 0;
	PID_Handler->differentiator = 0;
	PID_Handler->prevMeasurment = 0;
	PID_Handler->output = 0;
}



float PID_Update(PID *PID_H, float setpoint, float measurment)
{
	/*	Error calculate		*/

	float error = setpoint - measurment;

	/*	P	*/

	float proportional = PID_H->Kp * error;

	/*	I	*/

	PID_H->integrator = PID_H->integrator + ((PID_H->Ki * PID_H->T * 0.5) * ( error -  PID_H->prevError));

	/*	Limit	*/


}
