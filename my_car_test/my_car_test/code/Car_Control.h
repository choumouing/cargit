#ifndef __CAR_CONTROL_H
#define __CAR_CONTROL_H

#define DIR_L               (A0 )
#define PWM_L               (TIM5_PWM_CH2_A1)

#define DIR_R               (A2 )
#define PWM_R               (TIM5_PWM_CH4_A3)

void Motor_Init(void);       //DIR,PWM¿Ú³õÊ¼»¯
void Motor_Left_SetSpeed(int16_t duty);
void Motor_Right_SetSpeed(int16_t duty);
void CarControl_Turn(int8_t Speed,int16_t Difference);
void motor_setspeed(int16 target, float current_l, float current_r);

#endif
