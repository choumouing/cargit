#ifndef __PID_H__
#define __PID_H__

extern float gyro_z;
// 位置式PID结构
typedef struct 
{
    float Kp, Ki, Kd_d,Kd_a;
    float integral;  // 积分累计
    float prev_err;  // 上次误差
} PositionalPID;

// 增量式PID结构体
typedef struct 
{
    float Kp, Ki, Kd;
    float err[3];  // 环形队列: [0]=e(k-2), [1]=e(k-1), [2]=e(k)
} IncrementalPID;


// 位置式PID初始化
void PositionalPID_Init(PositionalPID* pid, float Kp, float Ki, float Kd_d,float Kd_a);
// 增量式PID初始化
void IncrementalPID_Init(IncrementalPID* pid, float Kp, float Ki, float Kd);

float PositionalPID_Update(PositionalPID* pid, float target, float current);
float IncrementalPID_Update(IncrementalPID* pid, float target, float current);

//int16_t PID_Speed(PositionalPID* P_pid,IncrementalPID* I_pid,const uint8_t *image);
#endif
