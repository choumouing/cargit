#include "mm32_device.h"                // Device header

float PID_Kp = 0,PID_Ki = 0,PID_Kd = 0;
float Err,Err_Last,Err_Sum,Err_Difference,P,I,D,Add,Value;

void PID_SetK(float Kp,float Ki,float Kd){
	Err_Sum = 0;
	PID_Kp = Kp;
	PID_Ki = Ki;
	PID_Kd = Kd;
}

void PID_Limit(float Limit){
	if(Value > 0 && Value > Limit){
		Value = Limit;
	}
	if(Value < 0 && Value < -Limit){
		Value = -Limit;
	}
}

float PID_Value(float Target,float Current){
	Err = Current - Target;
	Err_Sum += Err;
	Err_Difference = Err - Err_Last;
	Err_Last = Err;
	
	P = PID_Kp * Err;
	I = PID_Ki * Err_Sum;
	D = PID_Kd * Err_Difference;
	
	Value = P + I + D;
	PID_Limit(80);
	
	return Value;
	
}