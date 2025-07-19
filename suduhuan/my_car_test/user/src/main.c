/*********************************************************************************************************************
* MM32F327X-G8P Opensourec Library 即（MM32F327X-G8P 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
* 
* 本文件是 MM32F327X-G8P 开源库的一部分
* 
* MM32F327X-G8P 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
* 
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
* 
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
* 
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
* 
* 文件名称          main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          IAR 8.32.4 or MDK 5.37
* 适用平台          MM32F327X_G8P
* 店铺链接          https://seekfree.taobao.com/
* 
* 修改记录
* 日期              作者                备注
* 2022-08-10        Teternal            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#include "Car_Control.h"
#include "Encoder.h"
#include "auto_menu.h"
#include "key.h"
#include "search_line.h"
#include "PID.h"
#include "my_func.h"
#include "Find_Way.h"

// **************************** 代码区域 ****************************

			



#define DISPLAY_MODE                ( 0 )                                       // 显示模式 0-灰度显示 1-二值化显示 // 0-灰度显示   就是正常显示的总钻风图像 // 1-二值化显示 根据最后一个二值化阈值显示出对应的二值化图像
#define BINARIZATION_THRESHOLD      ( 64 )                                      // 二值化阈值 默认 64 需要设置 DISPLAY_MODE 为 1 才使用
#define IPS200_TYPE                 (IPS200_TYPE_SPI)                     // 双排排针 并口两寸屏 这里宏定义填写 IPS200_TYPE_PARALLEL8
                                                                                // 单排排针 SPI 两寸屏 这里宏定义填写 IPS200_TYPE_SPI
int16 encoder_data_left;
int16 encoder_data_right;

uint8 image_buffer[MT9V03X_H][MT9V03X_W]={0};

float position_error=0,speed_diff=0,current_speed_diff=0;
float speed_left_inc=0,speed_right_inc=0;
int16_t speed_left_final = 0,speed_right_final = 0;
int16_t speed_left_tar = 0,speed_right_tar = 0;
int16_t speed_left_base = 0,speed_right_base = 0;


PositionalPID position_pid = {0};
IncrementalPID speed_pid_l = {0};
IncrementalPID speed_pid_r = {0};
	
int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);                                              // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                                                             	// 初始化默认 Debug UART
	
		Motor_Init(); 	
		menu_init();
		mpu6050_init();	
    ips200_init(IPS200_TYPE);
		if_mt9v03x_init();

		Encoder_Init();
		pit_ms_init(PIT, 20);                                                      // 初始化 PIT 为周期中断 20ms 周期
		interrupt_set_priority(PIT_PRIORITY, 0);         	                           // 设置 PIT 对周期中断的中断优先级为 0

    while(1)
    {

				show_process(NULL);			
				IncrementalPID_Init(&speed_pid_l,s_kp,s_ki,s_kd);	
				IncrementalPID_Init(&speed_pid_r,s_kp,s_ki,s_kd);	
				PositionalPID_Init(&position_pid, p_kp,p_ki,p_kd);// 位置PID参数           1.6 0 0.8               2.6 0 2.8(67 60) 
				ips_show_mt9v03x(*image_buffer);   
			if(start_flag)
			{
		
				
				speed_diff = PositionalPID_Update(&position_pid,center_line_weight_final, 94);                    //位置PID的结果与速度差的关系
				
				if(speed_diff > 5000)speed_diff = 5000;
				if(speed_diff < -5000)speed_diff = -5000;			

				speed_left_tar = speed_base;
				speed_right_tar = speed_base;

				speed_left_inc = IncrementalPID_Update(&speed_pid_l,speed_left_tar,encoder_data_left);                       //速度环
				speed_right_inc = IncrementalPID_Update(&speed_pid_r,speed_right_tar,encoder_data_right);	
				if(speed_left_inc > 1000)speed_left_inc = 1000;
				if(speed_left_inc < -1000)speed_left_inc = -1000;
				if(speed_right_inc > 1000)speed_right_inc = 1000;
				if(speed_right_inc < -1000)speed_right_inc = -1000;				
				
				speed_left_base += speed_left_inc;
				speed_right_base += speed_right_inc;
				
				speed_left_final = speed_left_base + speed_diff;
				speed_right_final = speed_right_base - speed_diff;
				
				if(prospect < 5)
				{
					  speed_base = 0;
						speed_left_final = 0;
						speed_right_final = 0;
						start_flag = 0;
				}
				
		    Motor_Left_SetSpeed((int16_t)speed_left_final);			
		    Motor_Right_SetSpeed((int16_t)speed_right_final);
			}		
			if(start_flag == 0)
			{
				Motor_Left_SetSpeed(0);			
				Motor_Right_SetSpeed(0);							
			}


			//					ips200_show_int(0,170,center_line_weight_temp, 3);	
//					ips200_show_int(0,170,element_name, 3);	
					ips200_show_int(0,200,speed_left_tar, 3);				
					ips200_show_int(100,200,speed_right_tar, 3);
					ips200_show_int(0,200,speed_left_base, 3);				
					ips200_show_int(100,200,speed_right_base, 3);				
					ips200_show_int(0,230,speed_left_final, 3);				
					ips200_show_int(100,230,speed_right_final, 3);
					ips200_show_int(0,260,encoder_data_left, 3);				
					ips200_show_int(100,260,encoder_data_right, 3);				
//					ips200_show_int(0,260,reference_col_line[2], 3);		
//					ips200_show_int(0,200,left_variance, 3);	
//					ips200_show_int(0,230,right_variance, 3);	
//					ips200_show_int(60,230,prospect, 3);
//					ips200_show_int(60,260,top, 3);		
					ips200_show_int(0,290,center_line_weight_final, 3);	
//      	  printf("center_line_weight_temp \t\t%d .\r\n", center_line_weight_temp); 
//			 	  printf("target_speed_diff \t\t%f .\r\n", target_speed_diff); 
//					printf("speed_left \t\t%d .\r\n", encoder_data_left); 			
//					printf("speed_right \t\t%d .\r\n", encoder_data_right);
					printf("%d,%d,%d\n", encoder_data_left,encoder_data_right,400); 							
//				  printf("prospect \t\t%d .\r\n", prospect);
			  system_delay_ms(20);
    }
}



void pit_handler (void)
{
    encoder_data_left = encoder_get_count(ENCODER_QUADDEC_L);                  // 获取编码器计数
		encoder_data_left = encoder_data_left;
    encoder_clear_count(ENCODER_QUADDEC_L);                                    	// 清空编码器计数
		encoder_data_right = encoder_get_count(ENCODER_QUADDEC_R);
		encoder_data_right = - (encoder_data_right);
	  encoder_clear_count(ENCODER_QUADDEC_R); 
	


}
// **************************** 代码区域 ****************************
