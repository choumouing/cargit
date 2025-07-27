#include "zf_common_headfile.h"
#include "Car_Control.h"
#include "PID.h"

float speed_diff=0,speed_diff_l=0,speed_diff_r = 0;
float speed_left_inc=0,speed_right_inc=0;
int16_t speed_left = 0,speed_right = 0;
int16_t speed_left_base = 0,speed_right_base = 0;
int16_t speed_left_final = 0,speed_right_final = 0;

PID_INCREMENT_TypeDef speed_increment_left = {0};
PID_INCREMENT_TypeDef speed_increment_right = {0};

void Motor_Init(void)       //DIR,PWM�ڳ�ʼ��
{
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_L, 17000, 0);                                                

    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_R, 17000, 0);                                                
}


void Motor_Left_SetSpeed(int16_t duty)
{	
	if(duty > 5000)duty = 5000;
	if(duty < -5000)duty = -5000;        //
	
	if (duty >= 0)							//���������ת���ٶ�ֵ
	{
		gpio_set_level(DIR_L, GPIO_LOW);
		pwm_set_duty(PWM_L, duty);                  
                              
	}
	else									//���򣬼����÷�ת���ٶ�ֵ
	{  
		 gpio_set_level(DIR_L, GPIO_HIGH);
     pwm_set_duty(PWM_L, (-duty));
	}
}
void Motor_Right_SetSpeed(int16_t duty)
{
	if(duty > 5000)duty = 5000;
	if(duty < -5000)duty = -5000;
	if (duty >= 0)							//���������ת���ٶ�ֵ
	{
		gpio_set_level(DIR_R, GPIO_LOW);
		pwm_set_duty(PWM_R, duty);                  // ����ռ�ձ�           
	}
	else									//���򣬼����÷�ת���ٶ�ֵ
	{ 
		gpio_set_level(DIR_R, GPIO_HIGH);
    pwm_set_duty(PWM_R, (-duty));               // ����ռ��
	}
}

void CarControl_Turn(int8_t Speed,int16_t Difference)
{
	Motor_Left_SetSpeed(Speed+Difference);
	Motor_Right_SetSpeed(Speed-Difference);
}