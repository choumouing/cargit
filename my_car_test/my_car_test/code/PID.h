#ifndef __PID_H__
#define __PID_H__

void PID_SetK(float Kp,float Ki,float Kd);
float PID_Value(float Target,float Current);


#endif
