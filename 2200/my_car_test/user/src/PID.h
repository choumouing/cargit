#ifndef __PID_H__
#define __PID_H__

extern float gyro_z,prev_d;
// 位置式PID结构
typedef struct 
{
    float Kp_2,Kp_1,Kp_0,Ki,Kd_d,Kd_a;
    float integral;  // 积分累计
    float prev_err;  // 上次误差
		float prev_d;
} PositionalPID;

// 增量式PID结构体
typedef struct 
{
    float Kp, Ki, Kd;
    float err[3];  // 环形队列: [0]=e(k-2), [1]=e(k-1), [2]=e(k)
} IncrementalPID;

typedef struct {
    float last_error;      // 上一次误差
    float prev_error;      // 上上次误差（用于微分）
    float output;          // 当前输出值
	float last_i;          // 当前输出值

} PID_INCREMENT_TypeDef;

// 位置式PID初始化
void PositionalPID_Init(PositionalPID* pid,float Kp_2,float Kp_1,float Kp_0,float Ki,float Kd_d,float Kd_a);
// 增量式PID初始化
void IncrementalPID_Init(IncrementalPID* pid, float Kp, float Ki, float Kd);

float PositionalPID_Update(PositionalPID* pid, float target, float current);
float IncrementalPID_Update(IncrementalPID* pid, float target, float current);
float pid_increment(PID_INCREMENT_TypeDef *pid, float target, float current,float limit, float kp, float ki, float kd);
//int16_t PID_Speed(PositionalPID* P_pid,IncrementalPID* I_pid,const uint8_t *image);
#endif
