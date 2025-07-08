#include "mm32_device.h"                // Device header
#include "PID.h"
#include "Encoder.h"
#include "search_line.h"
#include "my_func.h"


// λ��ʽPID��ʼ��
void PositionalPID_Init(PositionalPID* pid, float Kp, float Ki, float Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
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
    // �������
    float err = current - target;
    
    // ��������� (�����ʹ���ͨ����������)
    pid->integral += err;
    
    // ΢���� (��ǰ���-�ϴ����)
    float derivative = err - pid->prev_err;
    
    // PID�������
    float output = pid->Kp * err 
                + pid->Ki * pid->integral 
                + pid->Kd * derivative;
    
    // ������ʷ���
    pid->prev_err = err;
    
    return output;
}



float IncrementalPID_Update(IncrementalPID* pid, float target, float current) 
{
    // ����������
    pid->err[0] = pid->err[1];  // e(k-2) = e(k-1)
    pid->err[1] = pid->err[2];  // e(k-1) = e(k)
    pid->err[2] = current - target;  // ��ǰ���e(k)
    
    // �������� (���Ĺ�ʽ)
    float delta = pid->Kp * (pid->err[2] - pid->err[1]) 
                + pid->Ki * pid->err[2] 
                + pid->Kd * (pid->err[2] - 2*pid->err[1] + pid->err[0]);
    
    return delta;  // ���ؿ�������
}

int16_t PID_Speed(PositionalPID* P_pid,IncrementalPID* I_pid,const uint8_t *image)
{

}