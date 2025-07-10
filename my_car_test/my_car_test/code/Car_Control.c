#include "zf_common_headfile.h"
#include "Car_Control.h"
#include "PID.h"


void Motor_Init(void)       //DIR,PWM�ڳ�ʼ��
{
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_L, 17000, 0);                                                

    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_R, 17000, 0);                                                
}


void Motor_Left_SetSpeed(int16_t duty)
{	
	if(duty > 80)duty = 80;
	if(duty < -80)duty = -80;        //
	
	if (duty >= 0)							//���������ת���ٶ�ֵ
	{
		gpio_set_level(DIR_L, GPIO_LOW);
		pwm_set_duty(PWM_L, duty*25 );                  
                              
	}
	else									//���򣬼����÷�ת���ٶ�ֵ
	{  
		 gpio_set_level(DIR_L, GPIO_HIGH);
     pwm_set_duty(PWM_L, (-duty)*25);
	}
}
void Motor_Right_SetSpeed(int16_t duty)
{
	if(duty > 80)duty = 80;
	if(duty < -80)duty = -80;
	if (duty >= 0)							//���������ת���ٶ�ֵ
	{
		gpio_set_level(DIR_R, GPIO_LOW);
		pwm_set_duty(PWM_R, duty*25);                  // ����ռ�ձ�           
	}
	else									//���򣬼����÷�ת���ٶ�ֵ
	{ 
		gpio_set_level(DIR_R, GPIO_HIGH);
    pwm_set_duty(PWM_R, (-duty)*25);               // ����ռ��
	}
}

void CarControl_Turn(int8_t Speed,int16_t Difference)
{
	Motor_Left_SetSpeed(Speed+Difference);
	Motor_Right_SetSpeed(Speed-Difference);
}