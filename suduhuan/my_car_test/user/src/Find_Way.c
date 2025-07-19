#include "mm32_device.h"                // Device header
#include "search_line.h"
#include "find_way.h"
#include "my_func.h"
#include "zf_device_mpu6050.h"
#include "globals.h"
 
uint8_t element_name = 0;
int16_t test_data1 = 0,test_data2 = 0,test_data3 = 0;
int16_t island_temp_flag = 0;
uint16 circle_time = 0;
float circle_gyro_z = 0;
float angle_temp = 0;
int encoder_temp = 0;


uint8_t left_half_line[SEARCH_IMAGE_H]={           
21,21,22,23,23,24,25,25,26,27,
27,28,29,29,30,30,31,32,33,33,
34,35,35,37,37,38,38,39,40,40,
41,42,42,43,44,44,45,45,46,47,
47,48,49,50,50,52,52,52,53,54,
55,55,56,57,57,58,59,59,60,60,
61,62,62,63,64,64,65,66,67,67,
68,69,70,70,71,72,72,73,74,74,
75,75,76,77,77,78,79,79,80,82,
81,82,83,84,84,85,86,86,87,88,
88,89,89,90,90,91,92,92,93,94,
95,95,96,97,97,98,99,99,100,101
};
uint8_t half_line[SEARCH_IMAGE_H]={           
 6, 6, 7, 8,8, 9,10,10,11,12,
12,13,14,14,15,15,16,17,18,18,
19,20,20,22,22,23,23,24,25,25,
26,27,27,28,29,29,30,30,31,32,
32,33,34,35,35,37,37,37,38,39,
40,40,41,42,42,43,44,44,45,45,
46,47,47,48,49,49,50,51,52,52,
53,54,55,55,56,57,57,58,59,59,
60,60,61,62,62,63,64,64,65,67,
66,67,68,69,69,70,71,71,72,73,
73,74,74,75,75,76,77,77,78,79,
80,80,81,82,82,83,84,84,85,86
};
//uint8_t right_half_line[SEARCH_IMAGE_H]={           
//	 1, 1, 2, 3, 3, 4, 5, 5, 6, 7,
//	 7, 8, 9, 9,10,10,11,12,13,13, 
//	14,15,15,17,17,18,18,19,20,20,
//	21,22,22,23,24,24,25,25,26,27,
//	27,28,29,30,30,32,32,32,33,34,
//	35,35,36,37,37,38,39,39,40,40,
//	41,42,42,43,44,44,45,46,47,47,
//	48,49,50,50,51,52,52,53,54,54,
//	55,55,56,57,57,58,59,59,60,62,
//	61,62,63,64,64,65,66,66,67,68,
//	68,69,69,70,70,71,72,72,73,74,
//	75,75,76,77,77,78,79,79,80,81 
//};


uint8_t find_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1, uint8_t jump_num, uint8_t model)
{
	uint8 temp_jump_point = 0;
	uint8 temp_data;
	
	if(model){
		temp_jump_point = num0;
		for(int i = 0;i <num0-num1;i++){
			temp_data = myabs(arrary_value[num0-i]-arrary_value[num0-i-2]);
			if(temp_data>jump_num){
				temp_jump_point = (uint8)(num0-i);
				return temp_jump_point;
			}
		}
	}
	else{
		temp_jump_point = num1;
		for(int i = 0;i <num0-num1;i++){
			temp_data = myabs(arrary_value[num1+i]-arrary_value[num1+i+2]);
			if(temp_data>jump_num){
				temp_jump_point = (uint8)(num1+i);
				return temp_jump_point;
			}
		}
	}
	return 0;
}

uint8_t find_circle_up_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1)
{	
		for(int i = num1;i <num0;i++)
		{
				if(
					arrary_value[i - 3] > arrary_value[i - 5] &&
					 arrary_value[i] > arrary_value[i - 3] &&
					(arrary_value[i + 3] - arrary_value[i] > 10)
				)
				return i;
			}
	return 0;
}

uint8_t find_circle_down_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1)
{
	for(int i = num0;i > num1;i--)
	{
		if(
			 arrary_value[i + 5] > arrary_value[i + 3] &&
			 arrary_value[i + 3] > arrary_value[i] &&
			(arrary_value[i - 3] - arrary_value[i] > 10) 
//			(myabs(arrary_value[i - 1] - arrary_value[i - 2]) < 5)&&
			)		return i;
	}
	return 0;
}
void connect_point(uint8_t *arrary_value,uint8_t num0,uint8_t num1)
{
	float point_1 = (float)arrary_value[num0];
	float point_2 = (float)arrary_value[num1];
	float temp_slope = (point_2 - point_1) / (num0 - num1);
	
	for(int i = 0;i < (num0 - num1);i++)
	{
		arrary_value[num0 - i] = (int8_t)(temp_slope * i) + arrary_value[num0];
	}
	
}

void calculate_half_way()
{
		for(int i = SEARCH_IMAGE_H-40;i>0;i--)
		{
			if(left_edge_line[i]<3)
				return;
		}
		for(int i = SEARCH_IMAGE_H-40;i>0;i--)
		{
			if(right_edge_line[i]>185)
				return;
		}
		if(prospect > 100 && top < 10)
		{
			for(int i = 0;i < SEARCH_IMAGE_H;i++)
			{
				half_line[i] = right_edge_line[i] - center_line[i];
			}
		}
	
}

uint8 image_find_circle_point(uint8_t *edge_line,uint8 down_num,uint8 up_num,uint8 model) //model == 1从小到大 model == 0从大到小
{
	uint8 temp_jump_point = 0;
	if(model)
	{
		for(int i = 0;i < (down_num - up_num - 2);i++)
		{
			if(
				edge_line[up_num + i]<edge_line[up_num + i - 1]&&
				edge_line[up_num + i]<edge_line[up_num + i + 3]&&
				edge_line[up_num + i]<edge_line[up_num + i + 5]
				)
			{
				temp_jump_point = (uint8)(up_num + i);	
				return temp_jump_point;
			}
		}
	
	}
	else
	{
		for(int i = 0;i < (down_num - up_num - 2);i++)
		{
			if(
				edge_line[down_num - i]<edge_line[down_num - i - 1]&&
				edge_line[down_num - i]<edge_line[down_num - i + 3]&&
				edge_line[down_num - i]<edge_line[down_num - i + 5]
				)
			{
				temp_jump_point = (uint8)(down_num - i);	
				return temp_jump_point;
			}
		}
	}

	return 0;
}


void get_center_weight()
{
	if(island_temp_flag)
	{
		memcpy(center_line_weight_buffer,center_line_weight,sizeof(center_line_weight));	
		center_line_weight_buffer[0] = 0;
		center_line_weight_buffer[1] = 0;
		center_line_weight_buffer[2] = 0;
		center_line_weight_buffer[3] = 0;
		center_line_weight_buffer[4] = 0;
		center_line_weight_buffer[5] = 0;
		center_line_weight_buffer[6] = 1;
		center_line_weight_buffer[7] = 5;
		center_line_weight_buffer[8] = 9;
		center_line_weight_buffer[9] = 5;		
		center_line_weight_buffer[10] = 1;	

	}
	else
		{
	
				memcpy(center_line_weight_buffer,center_line_weight,sizeof(center_line_weight));	
				for(int i = 0;i < 11;i++)
		    {	
					if((10+i*10) < top)
					{
						center_line_weight_buffer[i] = 0;
					}
		    }			
				for(int i = 0;i < 11;i++)
				{
					center_line_weight_temp += center_line_weight_buffer[i]*center_line[10+i*10];	
					center_line_weight_count += center_line_weight_buffer[i];
				}
				center_line_weight_final = center_line_weight_temp / center_line_weight_count;		
				center_line_weight_count = 0;
				center_line_weight_temp = 0;
		}

}

void FindIsland_Ready()
{
	if(island_temp_flag == 0)
	{
 
		for(int i = SEARCH_IMAGE_H-40;i>20;i--)
	  {
			if(myabs(left_edge_line[i]-left_edge_line[i+1])>5)
			{			
//				island_temp_flag = 10;
				return;
			}
		}
		for(int i = SEARCH_IMAGE_H-40;i>30;i--)
		{
			if(left_edge_line[i]<5)
			{
//				island_temp_flag = 20;
				return;
			}

		}	
		uint8 up_point = 0,end_point = 0;
		end_point = find_circle_down_jump_point(right_edge_line,SEARCH_IMAGE_H,40);
		if(end_point)
		{
//			island_temp_flag = 40;
			up_point = image_find_circle_point(right_edge_line,end_point - 5,0,1 );
		}
		if(end_point)
		{
					island_temp_flag = 1;
					center_line_mode = 2;
					encoder_temp = 0;
					angle_temp = 0;
		}
	}

}
void FinfIsland_In()
{
	if(island_temp_flag == 1 && encoder_temp > 2000)
	{
		island_temp_flag = 2;
		center_line_mode = 0;		
	}
}
void FindIsland_Ing()
{
 if(island_temp_flag == 2 && encoder_temp > 3200)
 {
	 island_temp_flag = 3;
	 center_line_mode = 0;		 
 }
}
void FindIsland_Outready()
{
	if(island_temp_flag == 3 && encoder_temp > 7300)
	{
		island_temp_flag = 4;
		center_line_mode = 0;	
//		encoder_temp = 0;
	}
}
void FindIsland_Out()
{
	if(island_temp_flag == 4 && encoder_temp > 9500)
	{
		island_temp_flag = 5;
		center_line_mode = 2;	
	}
}
void FindIsland_Complete()
{
	if(island_temp_flag == 5 && encoder_temp > 10000)
	{
		island_temp_flag = 0;
		center_line_mode = 0;	
	}
}
//void FindIsland_Out()
//{
//	if(island_temp_flag == 1 && encoder_temp > 1800)
//	{
//		center_line_mode = 1;
//		angle_temp = 0;
//		encoder_temp = 0;
//		island_temp_flag = 2;
//	}
//	if(island_temp_flag == 2 && angle_temp > 500)
//	{
//		center_line_mode = 0;
//		angle_temp = 0;
//		encoder_temp = 0;
//		island_temp_flag = 3;
//	}
//	if(island_temp_flag == 3 && angle_temp > 2770)
//	{
//		center_line_mode = 2;
//		angle_temp = 0;
//		encoder_temp = 0;	
//		island_temp_flag = 4;		
//	}
//	if(island_temp_flag == 4 && encoder_temp > 400)
//	{
//		center_line_mode = 0;
//		island_temp_flag = 0;
//	}
//}
//void FindIsland_Out()
//{
//	if(island_temp_flag == 1 && angle_temp > 366)
//	{
//		center_line_mode = 0;
//		angle_temp = 0;
//		encoder_temp = 0;
//		island_temp_flag = 2;
//	}
//	if(island_temp_flag == 2 && angle_temp > 1776)
//	{
//		center_line_mode = 1;
//		angle_temp = 0;
//		encoder_temp = 0;
//		island_temp_flag = 3;
//	}
//	if(island_temp_flag == 3 && angle_temp > 690)
//	{
//		center_line_mode = 2;
//		angle_temp = 0;
//		encoder_temp = 0;	
//		island_temp_flag = 4;		
//	}
//	if(island_temp_flag == 4 && encoder_temp > 500)
//	{
//		center_line_mode = 0;
//		island_temp_flag = 0;
//	}
//}




//void Connect_Cross_Ready(uint8_t *arrary_value1,uint8_t *arrary_value2)
//{
//		int16_t left_start_point = 0,left_end_point = 0,right_start_point = 0,right_end_point = 0;
//		float temp_slope = 0;
//	
//		left_start_point = find_jump_point(arrary_value1,100,20,CROSSJUMPTHRESHOLD,1) + 5;
//		left_end_point = find_jump_point(arrary_value1,100,20,CROSSJUMPTHRESHOLD,0) - 5;	
//		if((arrary_value1[left_end_point] - arrary_value1[left_start_point]) < 5)left_nomal_flag = 1;
////		end_point = find_jump_point(arrary_value1,start_point - 10,20,CROSSJUMPTHRESHOLD,1) - 5;

//		test_data2 = left_start_point - 5;
//		test_data3 = left_end_point + 5;

//		right_start_point = find_jump_point(arrary_value2,100,20,CROSSJUMPTHRESHOLD,1) + 5;
//		right_end_point = find_jump_point(arrary_value2,100,20,CROSSJUMPTHRESHOLD,0) - 5;
//		if((arrary_value1[right_start_point] - arrary_value1[right_end_point]) < 5)right_nomal_flag = 1;
////		end_point = find_jump_point(arrary_value2,start_point,CROSSDOWNEDGE,CROSSJUMPTHRESHOLD,1);
//		if(left_nomal_flag && right_nomal_flag)
//		{
//			connect_point(arrary_value1,left_start_point,left_end_point);
//			connect_point(arrary_value2,right_start_point,right_end_point);	
//		}
//		else if(left_nomal_flag && !right_nomal_flag)
//		{
//			connect_point(arrary_value1,left_start_point,left_end_point);		
//			temp_slope = ((float)arrary_value1[left_start_point] - (float)arrary_value1[left_end_point]) / (left_end_point - left_start_point);
//			for(int i = 0;i < (left_start_point - left_end_point);i++)
//			{
//				arrary_value2[left_start_point - i] = -(int8_t)((temp_slope) * i) + arrary_value2[left_start_point];
//				if(arrary_value2[left_start_point - i] >= 188)arrary_value2[left_start_point - i] = 188;
//				if(arrary_value2[left_start_point - i] <= 0)arrary_value2[left_start_point - i] = 0;
//			}			
//		}
//		else if(!left_nomal_flag && right_nomal_flag)
//		{
//			connect_point(arrary_value2,right_start_point,right_end_point);		
//			temp_slope = ((float)arrary_value2[right_start_point] - (float)arrary_value1[right_end_point]) / (right_end_point - right_start_point);
//			for(int i = 0;i < (right_start_point - right_end_point);i++)
//			{
//				arrary_value1[right_start_point - i] = -(int8_t)((temp_slope) * i) + arrary_value1[right_start_point];
//				if(arrary_value1[right_start_point - i] >= 188)arrary_value1[right_start_point - i] = 188;
//				if(arrary_value1[right_start_point - i] <= 0)arrary_value1[right_start_point - i] = 0;
//			}					
//		}
//}

void Connect_Cross_In(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	
		int16_t start_point,end_point;
		float temp_slope = 0;
		start_point = 120;
		end_point = find_jump_point(arrary_value1,100,20,CROSSJUMPTHRESHOLD,0);
		temp_slope = ((float)arrary_value1[end_point] - (float)arrary_value1[end_point - 20]) / (-20);
		for(int i = 0;i < (start_point - end_point);i++)
		{
			arrary_value1[end_point + i] = -(int8_t)((temp_slope) * i) + arrary_value1[end_point];
			if(arrary_value1[end_point + i] >= 188)arrary_value1[end_point + i] = 188;
			if(arrary_value1[end_point + i] <= 0)arrary_value1[end_point + i] = 0;
		}
		end_point = find_jump_point(arrary_value2,100,20,CROSSJUMPTHRESHOLD,0);
		temp_slope = ((float)arrary_value1[end_point] - (float)arrary_value1[end_point - 20]) / (-20);
		for(int i = 0;i < (start_point - end_point);i++)
		{
			arrary_value2[end_point + i] = (int8_t)((temp_slope) * i) + arrary_value2[end_point];
			if(arrary_value2[end_point + i] >= 188)arrary_value2[end_point + i] = 188;
			if(arrary_value2[end_point + i] <= 0)arrary_value2[end_point + i] = 0;
		}
		
}

void Connect_Circle_In()
{
	for(int i = 0;i < 120;i++)
	{
		left_edge_line[i] = 188 - 1.5 * i;
	}
	for(int i = 180;i < 188;i++)
	{
		left_edge_line[i] = 0;
	}
}

