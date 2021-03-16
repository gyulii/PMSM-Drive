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
	PID_Handler->limit_max_output = 65535;
	PID_Handler->limit_min_output = 0;
	PID_Handler->proportional = 0;
	PID_Handler->prevIntegrator = 0;
	/*	Tau, KP,KI,KD needed	Tsample	*/
}



float PID_Update(PID *PID_H, float setpoint, float measurment)
{
	/*	Error calculate		*/

	float error = setpoint - measurment;

	/*	P	*/

	PID_H->proportional = PID_H->Kp * error;

	/*	I	*/

	PID_H->integrator = PID_H->integrator + ((PID_H->Ki * PID_H->T * 0.5) * ( error -  PID_H->prevError));


    PID_H->differentiator = -(2.0 * PID_H->Kd * (measurment - PID_H->prevMeasurment)
                        + (2.0f * PID_H->tau - PID_H->T) * PID_H->differentiator)
                        / (2.0f * PID_H->tau + PID_H->T);



    PID_H->output = PID_H->proportional + PID_H->integrator + PID_H->differentiator;



	/*	Limit	*/


	PID_H->is_int_winded_up = 0;

	  if (PID_H->output > PID_H->limit_max_output)
	  {
		  PID_H->output = PID_H->output > PID_H->limit_max_output;

	    if (error > 0)
	    {
	    	PID_H->is_int_winded_up = 1;
	    }
	  }
	  else if (PID_H->output < PID_H->limit_min_output)
	  {
		  PID_H->output = PID_H->limit_min_output;

	    if (error < 0)
	    {
	    	PID_H->is_int_winded_up = 1;
	    }
	  }


	  if (PID_H->is_int_winded_up == 1) // Wind up == No I update
	  {
		  PID_H->integrator = PID_H->prevIntegrator;
	  }



/* Save memory blocks*/

	PID_H->prevIntegrator = PID_H->integrator;
	PID_H->prevMeasurment = measurment;
	PID_H -> prevError = error;

	return PID_H->output;

}
