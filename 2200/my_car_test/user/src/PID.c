#include "mm32_device.h"                // Device header
#include "PID.h"
#include "Encoder.h"
#include "search_line.h"
#include "my_func.h"
#include "zf_device_mpu6050.h"

float gyro_z = 0,prev_d = 0;
// 位置式PID初始化
void PositionalPID_Init(PositionalPID* pid,float Kp_2,float Kp_1,float Kp_0,float Ki,float Kd_d,float Kd_a) 
{
		pid->Kp_2 = Kp_2;
	  pid->Kp_1 = Kp_1;
    pid->Kp_0 = Kp_0;
    pid->Ki = Ki;
    pid->Kd_d = Kd_d;
		pid->Kd_a = Kd_a;
    pid->integral = 0.0f;
    pid->prev_err = 0.0f;
		pid->prev_d = 0.0f;
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
			mpu6050_get_gyro();  // 更新数据
			mpu6050_gyro_z = mpu6050_gyro_z + 117;
			if(myabs(mpu6050_gyro_z) <= 5)mpu6050_gyro_z = 0;
		
			// 计算误差
			float err = target - current;
			
			// 积分项更新 (防饱和处理通常加在这里)
			pid->integral += err;
			
			if(pid->integral > 3000)pid->integral = 3000;
			else if(pid->integral < -3000)pid->integral = -3000;
		
			// 微分项 (当前误差-上次误差)
			float derivative = err - pid->prev_err;

			// PID输出计算
			float output = pid->Kp_2 * err * err + pid->Kp_1 * err + pid->Kp_0
										+ pid->Ki * pid->integral 
										+ (1 - 0.1) * ( pid->Kd_a * (- mpu6050_gyro_z) / 1000 + pid->Kd_d * derivative ) + 0.1 * pid->prev_d;
			
			// 更新历史误差
			pid->prev_err = err;
			pid->prev_d = (1 - 0.1) * ( pid->Kd_a * (- mpu6050_gyro_z) / 1000 + pid->Kd_d * derivative ) + 0.1 * pid->prev_d;
							
			if(output > 7000)output = 7000;
			if(output < -7000)output = -7000;			
			return output;
	}



float IncrementalPID_Update(IncrementalPID* pid, float target, float current) 
{ 
    // 更新误差队列
    pid->err[0] = pid->err[1];  // e(k-2) = e(k-1)
    pid->err[1] = pid->err[2];  // e(k-1) = e(k)
    pid->err[2] = target - current;  // 当前误差e(k)
	
    // 计算增量 (核心公式)
    float delta = pid->Kp * (pid->err[2] - pid->err[1]) 
                + pid->Ki * pid->err[2] 
                + pid->Kd * (pid->err[2] - 2*pid->err[1] + pid->err[0]);

    
    return delta;  // 返回控制增量
}

float pid_increment(PID_INCREMENT_TypeDef *pid, float target, float current,float limit, float kp, float ki, float kd) 
{
    float error = target - current;
    
    float p_term = kp * (error - pid->last_error);
    float i_term = ki * error *0.9 + pid->last_i *0.1;       
    float d_term = kd * (error - 2*pid->last_error + pid->prev_error);
    float increment = p_term + i_term + d_term;
    pid->output += increment;
	pid->last_i = i_term;
    // 更新误差历史
    pid->prev_error = pid->last_error;
    pid->last_error = error;

    // 输出限幅
    if(pid->output > limit) pid->output = limit;
    else if(pid->output < -limit) pid->output = -limit;
    
    return pid->output;
}
	