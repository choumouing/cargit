#include "zf_common_headfile.h"
#include "Car_Control.h"
#include "PID.h"
#include "auto_menu.h"
#include "find_way.h"
#include "search_line.h"

float speed_diff=0,speed_diff_l=0,speed_diff_r = 0;
float speed_left_inc=0,speed_right_inc=0;
int16_t speed_left = 0,speed_right = 0;
int16_t speed_left_base = 0,speed_right_base = 0;
int16_t speed_left_tar = 0,speed_right_tar = 0;
int16_t speed_left_final = 0,speed_right_final = 0;
int16_t start_count=0;

void Motor_Init(void)       //DIR,PWM口初始化
{
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_L, 17000, 0);                                                

    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          
    pwm_init(PWM_R, 17000, 0);                                                
}


void Motor_Left_SetSpeed(int16_t duty)
{	
	if(duty > 8000)duty = 8000;
	if(duty < -8000)duty = -8000;        //
	
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
	if(duty > 8000)duty = 8000;
	if(duty < -8000)duty = -8000;
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

void Get_Speed_Diff()             //获取左右轮差速
{
	if(speed_diff > 0)
	{
		speed_diff_l = chasu * speed_diff;
	  speed_diff_r = (2 - chasu) * speed_diff;
	}
	else
	{
		speed_diff_l = (2 - chasu) * speed_diff;
		speed_diff_r = chasu * speed_diff;			
	}
}

void Get_Speed_Base()
{
//	if(speed_up)
//	{
//		speed_left_base = speed_beilv * speed_base;
//		speed_right_base = speed_beilv * speed_base;					
//	}
//	else
//	{
		speed_left_base = speed_base;
		speed_right_base = speed_base;
//	}	
}

void protect()
{
		if(prospect < 5)
		{
//			if(start_count > 200)
//			{
					speed_base = 0;
					speed_left_base = 0;
					speed_right_base = 0;
					speed_left_final = 0;
					speed_right_final = 0;
					start_flag = 0;
					island_temp_flag = 0;
					start_count = 0;
//			}
		}	
}