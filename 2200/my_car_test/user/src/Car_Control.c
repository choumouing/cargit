#include "zf_common_headfile.h"
#include "Car_Control.h"
#include "PID.h"



void Motor_Init(void)       //DIR,PWM口初始化
{
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_L, 17000, 0);                                                

    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_R, 17000, 0);                                                
}


void Motor_Left_SetSpeed(int16_t duty)
{	
	if(duty > 6000)duty = 6000;
	if(duty < -6000)duty = -6000;        //
	
	if (duty >= 0)							//如果设置正转的速度值
	{
		gpio_set_level(DIR_L, GPIO_LOW);
		pwm_set_duty(PWM_L, duty);                  
                              
	}
	else									//否则，即设置反转的速度值
	{  
		 gpio_set_level(DIR_L, GPIO_HIGH);
     pwm_set_duty(PWM_L, (-duty));
	}
}
void Motor_Right_SetSpeed(int16_t duty)
{
	if(duty > 6000)duty = 6000;
	if(duty < -6000)duty = -6000;
	if (duty >= 0)							//如果设置正转的速度值
	{
		gpio_set_level(DIR_R, GPIO_LOW);
		pwm_set_duty(PWM_R, duty);                  // 计算占空比           
	}
	else									//否则，即设置反转的速度值
	{ 
		gpio_set_level(DIR_R, GPIO_HIGH);
    pwm_set_duty(PWM_R, (-duty));               // 计算占空
	}
}

void CarControl_Turn(int8_t Speed,int16_t Difference)
{
	Motor_Left_SetSpeed(Speed+Difference);
	Motor_Right_SetSpeed(Speed-Difference);
}