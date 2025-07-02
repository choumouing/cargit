#include "zf_common_headfile.h"
#include "Car_Control.h"

void Motor_Init(void)       //DIR,PWM口初始化
{
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_L, 17000, 0);                                                

    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_R, 17000, 0);                                                
}
void Motor_Left_SetSpeed(uint8_t Speed)
{
	Speed=Speed*0.01;
	if (Speed >= 0)							//正转
	{
		gpio_set_level(DIR_L, GPIO_HIGH);
		pwm_set_duty(PWM_L, (uint32)Speed*PWM_DUTY_MAX);                  
                              
	}
	else									      //反转
	{  
		 gpio_set_level(DIR_L, GPIO_LOW);
     pwm_set_duty(PWM_L,(uint32)(-Speed)*PWM_DUTY_MAX);
	}
}
void Motor_Right_SetSpeed(uint8_t Speed)
{
	Speed=Speed*0.01;
	if (Speed >= 0)							//正转
	{
		gpio_set_level(DIR_R, GPIO_HIGH);
		pwm_set_duty(PWM_R, (uint32)Speed*PWM_DUTY_MAX);                  
                              
	}
	else									      //反转
	{  
		 gpio_set_level(DIR_R, GPIO_LOW);
     pwm_set_duty(PWM_R,(uint32)(-Speed)*PWM_DUTY_MAX);
	}
}
void CarControl_Turn(uint8_t Speed,int16_t Difference)
{
	Motor_Left_SetSpeed(Speed+Difference);
	Motor_Right_SetSpeed(Speed-Difference);
}