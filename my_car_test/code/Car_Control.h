#ifndef __CAR_CONTROL_H
#define __CAR_CONTROL_H

#define DIR_L      A0
#define PWM_L      TIM5_PWM_CH2_A1
#define DIR_R      A2 
#define PWM_R      TIM5_PWM_CH4_A3

void Motor_Init(void);       //DIR,PWM¿Ú³õÊ¼»¯
void Motor_Left_SetSpeed(uint8_t Speed);
void Motor_Right_SetSpeed(uint8_t Speed);
void CarControl_Turn(uint8_t Speed,int16_t Difference);

#endif
