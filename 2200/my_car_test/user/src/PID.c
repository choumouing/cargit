#include "mm32_device.h"                // Device header
#include "PID.h"
#include "Encoder.h"
#include "search_line.h"
#include "my_func.h"
#include "zf_device_mpu6050.h"

float gyro_z = 0,prev_derivative = 0;
// λ��ʽPID��ʼ��
void PositionalPID_Init(PositionalPID* pid, float Kp, float Ki, float Kd_d,float Kd_a) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd_d = Kd_d;
		pid->Kd_a = Kd_a;
    pid->integral = 0.0f;
    pid->prev_err = 0.0f;
}

// ����ʽPID��ʼ��
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
		mpu6050_get_gyro();  // ��������
		mpu6050_gyro_z = mpu6050_gyro_z + 117;
		if(myabs(mpu6050_gyro_z) <= 5)mpu6050_gyro_z = 0;
	
    // �������
    float err = target - current;
    
    // ��������� (�����ʹ���ͨ����������)
    pid->integral += err;
    
    // ΢���� (��ǰ���-�ϴ����)
    float derivative = err - pid->prev_err;
		derivative = (1 - 0.7) * derivative + 0.7 * prev_derivative;
		prev_derivative = derivative;
    
    // PID�������
    float output = pid->Kp * err 
                + pid->Ki * pid->integral 
                + pid->Kd_a * (- mpu6050_gyro_z) / 100 + pid->Kd_d * derivative;
    
    // ������ʷ���
    pid->prev_err = err;
    
    return output;
}



float IncrementalPID_Update(IncrementalPID* pid, float target, float current) 
{ 
    // ����������
    pid->err[0] = pid->err[1];  // e(k-2) = e(k-1)
    pid->err[1] = pid->err[2];  // e(k-1) = e(k)
    pid->err[2] = target - current;  // ��ǰ���e(k)
	
    // �������� (���Ĺ�ʽ)
    float delta = pid->Kp * (pid->err[2] - pid->err[1]) 
                + pid->Ki * pid->err[2] 
                + pid->Kd * (pid->err[2] - 2*pid->err[1] + pid->err[0]);

    
    return delta;  // ���ؿ�������
}

	