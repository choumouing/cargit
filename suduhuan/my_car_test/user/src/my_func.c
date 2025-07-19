#include "zf_common_headfile.h"
#include "search_line.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ips200.h"
#include "Find_Way.h"
#include "my_func.h"
char Image_Ready=0;

uint8_t center_line[SEARCH_IMAGE_H]={0};
uint8_t center_line_mode = 0;            //1为寻右边线,2为寻左边线

int myabs(int num)
{
	if(num>=0)return num;
	else return (-num);
}
float myabs_float(float num)
{
	if(num>=0)return num;
	else return (-num);
}

uint16_t calculate_variance(uint8_t array[], int size) 
{
    // Step 1: 计算平均值
    float sum = 0;
    for (int i = 0; i < size; i++) 
		{
        sum += array[i];
    }
    float mean = sum / size;

    // Step 2: 计算方差
    float variance = 0;
    for (int i = 0; i < size; i++)
		{
        float diff = array[i] - mean;
        variance += diff * diff;
    }
    variance /= size;

    return (uint16_t)variance;
}

void Update_Line(const uint8_t *image)
{
	Get_Reference_Point(image);
	Search_Reference_Col(image);
	Search_Line(image);
	FindIsland_Ready();
	FinfIsland_In();
	FindIsland_Ing();
	FindIsland_Outready();
	FindIsland_Out();
	FindIsland_Complete();
	get_center_weight();
	if(island_temp_flag == 2 || island_temp_flag == 4)
	{
		Connect_Circle_In();
	}
	if(center_line_mode == 1)
	{
		for(int i = 0;i < SEARCH_IMAGE_H;i++)
		{
			center_line[i]=right_edge_line[i] - half_line[i];
		}
	}
	if(center_line_mode == 2)
	{
		for(int i = 0;i < SEARCH_IMAGE_H;i++)
		{
			center_line[i]=left_edge_line[i] + left_half_line[i];
		}		
	}
	if(center_line_mode == 0)
	{
		for(int i = 0;i < SEARCH_IMAGE_H;i++)
		{
			center_line[i]=(left_edge_line[i] + right_edge_line[i])/2;
		}
	}
	for(int i = 0; i < SEARCH_IMAGE_H;i++)
	{
		if(center_line[i] > 188)center_line[i] = 188;
		if(center_line[i] < 0) center_line[i] = 0;
	}
	image_calculate_prospect(image);
	Find_Edge_At_Reference_Col(image);
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
					
//					ips200_show_gray_image(0, 0, (const uint8 *)image_buffer, MT9V03X_W, MT9V03X_H, 188, 120, 0);
//					for(uint16_t i=1;i<SEARCH_IMAGE_H;i++)
//						{
//							ips200_draw_point(left_edge_line[i],i,RGB565_RED);
//							ips200_draw_point(right_edge_line[i],i,RGB565_BLUE);
//							ips200_draw_point(center_line[i],i,RGB565_GREEN);
//							ips200_draw_point(reference_col,i,RGB565_BROWN);							
//						}
					Image_Ready=0;
				}
}