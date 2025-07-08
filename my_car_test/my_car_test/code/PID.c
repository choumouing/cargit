#include "mm32_device.h"                // Device header
#include "PID.h"
#include "Encoder.h"
#include "search_line.h"
#include "my_func.h"


// 位置式PID初始化
void PositionalPID_Init(PositionalPID* pid, float Kp, float Ki, float Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_err = 0.0f;
}

// 增量式PID初始化
void IncrementalPID_Init(IncrementalPID* pid, float Kp, float Ki, float Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    for(int i = 0; i < 3; i++) {
        pid->err[i] = 0.0f;
    }
}

float PositionalPID_Update(PositionalPID* pid, float target, float current) 
{
    // 计算误差
    float err = current - target;
    
    // 积分项更新 (防饱和处理通常加在这里)
    pid->integral += err;
    
    // 微分项 (当前误差-上次误差)
    float derivative = err - pid->prev_err;
    
    // PID输出计算
    float output = pid->Kp * err 
                + pid->Ki * pid->integral 
                + pid->Kd * derivative;
    
    // 更新历史误差
    pid->prev_err = err;
    
    return output;
}



float IncrementalPID_Update(IncrementalPID* pid, float target, float current) 
{
    // 更新误差队列
    pid->err[0] = pid->err[1];  // e(k-2) = e(k-1)
    pid->err[1] = pid->err[2];  // e(k-1) = e(k)
    pid->err[2] = current - target;  // 当前误差e(k)
    
    // 计算增量 (核心公式)
    float delta = pid->Kp * (pid->err[2] - pid->err[1]) 
                + pid->Ki * pid->err[2] 
                + pid->Kd * (pid->err[2] - 2*pid->err[1] + pid->err[0]);
    
    return delta;  // 返回控制增量
}

int16_t PID_Speed(PositionalPID* P_pid,IncrementalPID* I_pid,const uint8_t *image)
{

}