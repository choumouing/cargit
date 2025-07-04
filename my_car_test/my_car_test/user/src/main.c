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


// **************************** 代码区域 ****************************

			
#define PIT                             (TIM6_PIT )                             // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
#define PIT_PRIORITY                    (TIM6_IRQn)                             // 对应周期中断的中断编号 在 mm32f3277gx.h 头文件中查看 IRQn_Type 枚举体
	
#define DISPLAY_MODE                ( 0 )                                       // 显示模式 0-灰度显示 1-二值化显示
                                                                                // 0-灰度显示   就是正常显示的总钻风图像
                                                                                // 1-二值化显示 根据最后一个二值化阈值显示出对应的二值化图像
#define BINARIZATION_THRESHOLD      ( 64 )                                      // 二值化阈值 默认 64 需要设置 DISPLAY_MODE 为 1 才使用

#define IPS200_TYPE                 (IPS200_TYPE_SPI)                     // 双排排针 并口两寸屏 这里宏定义填写 IPS200_TYPE_PARALLEL8
                                                                                // 单排排针 SPI 两寸屏 这里宏定义填写 IPS200_TYPE_SPI
int16 encoder_data_quaddec = 0;

uint8 image_buffer[MT9V03X_H][MT9V03X_W]={0};

char Image_Ready=0;

int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);                                              // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                                                             	// 初始化默认 Debug UART
	
    ips200_init(IPS200_TYPE);
//    ips200_show_string(0, 0, "mt9v03x init.");
	
    encoder_quad_init(ENCODER_QUADDEC_R , ENCODER_QUADDEC_R_A, ENCODER_QUADDEC_R_B);   // 初始化编码器模块与引脚 正交解码编码器模式

    pit_ms_init(PIT, 100);                                                      // 初始化 PIT 为周期中断 100ms 周期

    interrupt_set_priority(PIT_PRIORITY, 0);                                    // 设置 PIT 对周期中断的中断优先级为 0
	
    while(1)
    {
        if(mt9v03x_init())
        {
            ips200_show_string(0, 16, "mt9v03x reinit.");
        }
        else
        {
            break;
        }
        system_delay_ms(500);                                                   // 短延时快速闪灯表示异常
    }
    ips200_show_string(0, 16, "init success.");

    while(1)
    {
        if(mt9v03x_finish_flag)
        {
#if (0 == DISPLAY_MODE)
					if(Image_Ready==0)
					{
						memcpy(image_buffer,mt9v03x_image, MT9V03X_W * MT9V03X_H);
						Image_Ready=1;
					}
            // 这是 ips200_displayimage03x 调用的真实函数 参数意义在其函数头有详细注释
						//  ips200_show_gray_image(0, 0, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 240, 180, 0);
#else
            // 二值化图像显示 参数意义在其函数头有详细注释
            // 二值化图像显示 参数意义在其函数头有详细注释
            // 二值化图像显示 参数意义在其函数头有详细注释
            ips200_show_gray_image(0, 0, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 240, 180, BINARIZATION_THRESHOLD);
#endif
            mt9v03x_finish_flag = 0;
        }
				if(Image_Ready==1)
				{
					Get_Reference_Point(*image_buffer);	
					Search_Reference_Col(*image_buffer);
					Search_Line(*image_buffer);
					ips200_displayimage03x(*image_buffer, 188, 120);            // 灰度图像显示 想要修改显示范围就修改本函数后两个参数 分别是显示宽度和高		
					for(uint16_t i=1;i<SEARCH_IMAGE_H;i++)
						{
							ips200_draw_point (left_edge_line[i],i,RGB565_RED);
 
							ips200_draw_point (right_edge_line[i],i,RGB565_BLUE);							
						}
					Image_Ready=0;
				}

    }
}




void pit_handler (void)
{
    encoder_data_quaddec = encoder_get_count(ENCODER_QUADDEC_R);                  // 获取编码器计数

    encoder_clear_count(ENCODER_QUADDEC_R);                                       // 清空编码器计数
}
// **************************** 代码区域 ****************************
