#ifndef __PID_H__
#define __PID_H__

extern float gyro_z;
// λ��ʽPID�ṹ
typedef struct 
{
    float Kp, Ki, Kd_d,Kd_a;
    float integral;  // �����ۼ�
    float prev_err;  // �ϴ����
} PositionalPID;

// ����ʽPID�ṹ��
typedef struct 
{
    float Kp, Ki, Kd;
    float err[3];  // ���ζ���: [0]=e(k-2), [1]=e(k-1), [2]=e(k)
} IncrementalPID;


// λ��ʽPID��ʼ��
void PositionalPID_Init(PositionalPID* pid, float Kp, float Ki, float Kd_d,float Kd_a);
// ����ʽPID��ʼ��
void IncrementalPID_Init(IncrementalPID* pid, float Kp, float Ki, float Kd);

float PositionalPID_Update(PositionalPID* pid, float target, float current);
float IncrementalPID_Update(IncrementalPID* pid, float target, float current);

//int16_t PID_Speed(PositionalPID* P_pid,IncrementalPID* I_pid,const uint8_t *image);
#endif
