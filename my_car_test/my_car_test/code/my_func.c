#include "zf_common_headfile.h"
#include "search_line.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ips200.h"
#include "Find_Way.h"
#include "my_func.h"

uint8_t center_line[SEARCH_IMAGE_H]={0};
//uint8_t weight[SEARCH_IMAGE_H]={1};


int myabs(int num)
{
	if(num>=0)return num;
	else return (-num);
}



void Update_Line(const uint8_t *image)
{
	Get_Reference_Point(image);
	Search_Reference_Col(image);
	Search_Line(image);
//	Judging_Elements(left_edge_line,right_edge_line);
//	Connect(left_edge_line,right_edge_line);
	for(int i = 0;i < SEARCH_IMAGE_H;i++)
	{
	center_line[i]=(left_edge_line[i] + right_edge_line[i])/2;
	}
}

void if_mt9v03x_init()							  //摄像头初始化
{
		while(1)                                      
    {
        if(mt9v03x_init()) ips200_show_string(0, 16, "mt9v03x reinit.");
        else break;
        system_delay_ms(500);                                                  
    }
    ips200_show_string(0, 16, "init success.");
}

void ips_show_mt9v03x(uint8_t *image_buffer)
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
					Update_Line(image_buffer);
//					ips200_displayimage03x(image_buffer, 188, 120);            // 灰度图像显示 想要修改显示范围就修改本函数后两个参数 分别是显示宽度和高		
					ips200_show_gray_image(0, 0, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 188, 120, 0);
					for(uint16_t i=1;i<SEARCH_IMAGE_H;i++)
						{
							ips200_draw_point(left_edge_line[i],i,RGB565_RED);
							ips200_draw_point(right_edge_line[i],i,RGB565_BLUE);
							ips200_draw_point(center_line[i],i,RGB565_GREEN);
							ips200_draw_point(reference_col_line[i],i,RGB565_BROWN);							
						}
					Image_Ready=0;
				}
}