#ifndef __CAR_CONTROL_H
#define __CAR_CONTROL_H

#define DIR_L               (A0 )
#define PWM_L               (TIM5_PWM_CH2_A1)

#define DIR_R               (A2 )
#define PWM_R               (TIM5_PWM_CH4_A3)

extern float speed_diff,speed_diff_l,speed_diff_r;
extern float speed_left_inc,speed_right_inc;
extern int16_t speed_left,speed_right;
extern int16_t speed_left_base,speed_right_base;
extern int16_t speed_left_tar,speed_right_tar;
extern int16_t speed_left_final,speed_right_final;
extern int16_t start_count;


void Motor_Init(void);       //DIR,PWM�ڳ�ʼ��
void Motor_Left_SetSpeed(int16_t duty);
void Motor_Right_SetSpeed(int16_t duty);
void CarControl_Turn(int8_t Speed,int16_t Difference);
void Get_Speed_Diff();
void Get_Speed_Base();
void protect();
#endif
