#include "mm32_device.h"                // Device header
#include "search_line.h"
#include "find_way.h"
#include "my_func.h"
#include "zf_device_mpu6050.h"
#include "globals.h"
#include "zf_common_function.h"

#define STRETCH_NUM       80          //补线延伸长度
//#include "auto_menu.h"
 
int circle_flag = 0;
uint8_t element_name = 0;
int16_t test_data1 = 0,test_data2 = 0,test_data3 = 0;
int16_t left_nomal_flag = 0,right_nomal_flag = 0;
int16_t island_temp_flag = 0;
uint16 circle_time = 0;
float circle_gyro_z = 0;
float angle_temp = 0;
int encoder_temp_left = 0;
int encoder_temp_right = 0;
uint8 cross_flag = 0;                    //十字标志位
uint8 banmaxian_flag = 0;
uint8 obstacle_flag = 0;
uint8 speed_up = 0;

uint8_t big_half_line[SEARCH_IMAGE_H]={           
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

uint8_t find_circle_down_jump_point_right(uint8_t *arrary_value, uint8_t num0, uint8_t num1)
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

uint8_t find_circle_down_jump_point_left(uint8_t *arrary_value, uint8_t num0, uint8_t num1)
{
	for(int i = num0;i > num1;i--)
	{
		if(
			 arrary_value[i + 5] < arrary_value[i + 3] &&
			 arrary_value[i + 3] < arrary_value[i] &&
			(arrary_value[i] - arrary_value[i - 3] > 10) 
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

uint8 find_circle_point_right(uint8_t *edge_line,uint8 down_num,uint8 up_num,uint8 model) //model == 1从小到大 model == 0从大到小
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

uint8 find_circle_point_left(uint8_t *edge_line,uint8 down_num,uint8 up_num,uint8 model) //model == 1从小到大 model == 0从大到小
{
	uint8 temp_jump_point = 0;
	if(model)
	{
		for(int i = 0;i < (down_num - up_num - 2);i++)
		{
			if(
				edge_line[up_num + i] > edge_line[up_num + i - 1]&&
				edge_line[up_num + i] > edge_line[up_num + i + 3]&&
				edge_line[up_num + i] > edge_line[up_num + i + 5]
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
				edge_line[down_num - i] > edge_line[down_num - i - 1]&&
				edge_line[down_num - i] > edge_line[down_num - i + 3]&&
				edge_line[down_num - i] > edge_line[down_num - i + 5]
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
		center_line_weight_buffer[0] = 0;
		center_line_weight_buffer[1] = 0;
		center_line_weight_buffer[2] = 1;
		center_line_weight_buffer[3] = 3;
		center_line_weight_buffer[4] = 5;
		center_line_weight_buffer[5] = 9;
		center_line_weight_buffer[6] = 5;
		center_line_weight_buffer[7] = 3;
		center_line_weight_buffer[8] = 1;
		center_line_weight_buffer[9] = 0;		
		center_line_weight_buffer[10] = 0;	

	}
	else
	{					
		memcpy(center_line_weight_buffer,center_line_weight,sizeof(center_line_weight));
	}			
				
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

void FindIsland_Ready_Right()
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
		end_point = find_circle_down_jump_point_right(right_edge_line,SEARCH_IMAGE_H,40);
		if(end_point)
		{
//			island_temp_flag = 40;
			up_point = find_circle_point_right(right_edge_line,end_point - 5,0,1 );
		}
		if(end_point)
		{
					island_temp_flag = 1;
					center_line_mode = 2;
					encoder_temp_left = 0;
					angle_temp = 0;
		}
	}

}
void FindIsland_Ready_Left()
{
	if(island_temp_flag == 0)
	{
 
		for(int i = SEARCH_IMAGE_H-40;i>20;i--)
	  {
			if(myabs(right_edge_line[i + 1]-right_edge_line[i]) > 5)
			{			
//				island_temp_flag = 10;
				return;
			}
		}
		for(int i = SEARCH_IMAGE_H-40;i>30;i--)
		{
			if(right_edge_line[i] > 183)
			{
//				island_temp_flag = 20;
				return;
			}
		}	
		uint8 up_point = 0,end_point = 0;
		end_point = find_circle_down_jump_point_left(left_edge_line,SEARCH_IMAGE_H,40);
		if(end_point)
		{
//			island_temp_flag = 40;
			up_point = find_circle_point_left(left_edge_line,end_point - 5,0,1 );
		}
		if(end_point)
		{
					island_temp_flag = 1;
					center_line_mode = 1;
					encoder_temp_right = 0;
					angle_temp = 0;
		}
	}

}


void FinfIsland_In_Right()
{
	if(island_temp_flag == 1 && encoder_temp_left > 10000)
	{
		island_temp_flag = 2;
		center_line_mode = 0;		
	}
}
void FinfIsland_In_Left()
{
	if(island_temp_flag == 1 && encoder_temp_right > 10000)
	{
		island_temp_flag = 2;
		center_line_mode = 0;		
	}
}


void FindIsland_Ing_Right()
{
 if(island_temp_flag == 2 && encoder_temp_left > 21000)
 {
	 island_temp_flag = 3;
	 center_line_mode = 0;		 
 }
}

void FindIsland_Ing_Left()
{
 if(island_temp_flag == 2 && encoder_temp_right > 21000)
 {
	 island_temp_flag = 3;
	 center_line_mode = 0;		 
 }
}


void FindIsland_Outready_Right()
{
	if(island_temp_flag == 3 && encoder_temp_left > 38000)
	{
		island_temp_flag = 4;
		center_line_mode = 0;	
	}
}
void FindIsland_Outready_Left()
{
	if(island_temp_flag == 3 && encoder_temp_right > 38000)
	{
		island_temp_flag = 4;
		center_line_mode = 0;	
	}
}


void FindIsland_Out_Right()
{
	if(island_temp_flag == 4 && encoder_temp_left > 59000)
	{
		island_temp_flag = 5;
		center_line_mode = 2;	
	}
}
void FindIsland_Out_Left()
{
	if(island_temp_flag == 4 && encoder_temp_right > 59000)
	{
		island_temp_flag = 5;
		center_line_mode = 1;	
	}
}


void FindIsland_Complete_Right()
{
	if(island_temp_flag == 5 && encoder_temp_left > 62000)
	{
		island_temp_flag = 0;
		center_line_mode = 0;	
	}
}
void FindIsland_Complete_Left()
{
	if(island_temp_flag == 5 && encoder_temp_right > 62000)
	{
		island_temp_flag = 0;
		center_line_mode = 0;	
	}
}

void circle_state()
{
	if(circle_flag == 1)              //右圆环
	{
		FindIsland_Ready_Right();
		FinfIsland_In_Right();
		FindIsland_Ing_Right();
		FindIsland_Outready_Right();
		FindIsland_Out_Right();
		FindIsland_Complete_Right();
	}
	else if(circle_flag == -1)       //左圆环
	{
		FindIsland_Ready_Left();
		FinfIsland_In_Left();
		FindIsland_Ing_Left();
		FindIsland_Outready_Left();
		FindIsland_Out_Left();
		FindIsland_Complete_Left();		
	}
}

void Connect_Cross_Ready(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
		int16_t left_start_point = 0,left_end_point = 0,right_start_point = 0,right_end_point = 0;
		float temp_slope = 0;
	
		left_start_point = find_jump_point(arrary_value1,100,20,CROSSJUMPTHRESHOLD,1) + 5;
		left_end_point = find_jump_point(arrary_value1,100,20,CROSSJUMPTHRESHOLD,0) - 5;	
		if((arrary_value1[left_end_point] - arrary_value1[left_start_point]) < 5)left_nomal_flag = 1;
//		end_point = find_jump_point(arrary_value1,start_point - 10,20,CROSSJUMPTHRESHOLD,1) - 5;

		test_data2 = left_start_point - 5;
		test_data3 = left_end_point + 5;

		right_start_point = find_jump_point(arrary_value2,100,20,CROSSJUMPTHRESHOLD,1) + 5;
		right_end_point = find_jump_point(arrary_value2,100,20,CROSSJUMPTHRESHOLD,0) - 5;
		if((arrary_value1[right_start_point] - arrary_value1[right_end_point]) < 5)right_nomal_flag = 1;
//		end_point = find_jump_point(arrary_value2,start_point,CROSSDOWNEDGE,CROSSJUMPTHRESHOLD,1);
		if(left_nomal_flag && right_nomal_flag)
		{
			connect_point(arrary_value1,left_start_point,left_end_point);
			connect_point(arrary_value2,right_start_point,right_end_point);	
		}
		else if(left_nomal_flag && !right_nomal_flag)
		{
			connect_point(arrary_value1,left_start_point,left_end_point);		
			temp_slope = ((float)arrary_value1[left_start_point] - (float)arrary_value1[left_end_point]) / (left_end_point - left_start_point);
			for(int i = 0;i < (left_start_point - left_end_point);i++)
			{
				arrary_value2[left_start_point - i] = -(int8_t)((temp_slope) * i) + arrary_value2[left_start_point];
				if(arrary_value2[left_start_point - i] >= 188)arrary_value2[left_start_point - i] = 188;
				if(arrary_value2[left_start_point - i] <= 0)arrary_value2[left_start_point - i] = 0;
			}			
		}
		else if(!left_nomal_flag && right_nomal_flag)
		{
			connect_point(arrary_value2,right_start_point,right_end_point);		
			temp_slope = ((float)arrary_value2[right_start_point] - (float)arrary_value1[right_end_point]) / (right_end_point - right_start_point);
			for(int i = 0;i < (right_start_point - right_end_point);i++)
			{
				arrary_value1[right_start_point - i] = -(int8_t)((temp_slope) * i) + arrary_value1[right_start_point];
				if(arrary_value1[right_start_point - i] >= 188)arrary_value1[right_start_point - i] = 188;
				if(arrary_value1[right_start_point - i] <= 0)arrary_value1[right_start_point - i] = 0;
			}					
		}
}

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

void stretch_point(uint8 *array_value, uint8 num, uint8 direction)
{
    if((num + 5 >= SEARCH_IMAGE_H) || (num - 5 <= 0))
        return;
    
    float temp_slope = 0;
    float point_1 = (float)array_value[num];
    
    if(direction){
        float point_2 = (float)array_value[num + 5];
        temp_slope = (point_1 - point_2) / 5;
        for(int i = 0; i < STRETCH_NUM && num - i >= 5; i++){
            array_value[num - i] = func_limit_ab((int8)(temp_slope * i) + array_value[num], 0, SEARCH_IMAGE_W - 1);
        }
    }
    else{
        float point_2 = (float)array_value[num - 5];
        temp_slope = (point_1 - point_2) / 5;
        for(int i = 0; i < STRETCH_NUM && num + i <= SEARCH_IMAGE_H - 1; i++){
            array_value[num + i] = func_limit_ab((int8)(temp_slope * i) + array_value[num], 0, SEARCH_IMAGE_W - 1);
        }
    }
}

uint8 find_right_jump_point(uint8 down_num, uint8 up_num, uint8 model)
{
    uint8 temp_jump_point = 0;
    
    if(model){
        temp_jump_point = down_num;
        for(int i = 0; i < down_num - up_num; i++){
            if(right_edge_line[down_num - i] - right_edge_line[down_num - i - 5] <= -8 &&
               right_edge_line[down_num - i] - right_edge_line[down_num - i - 6] <= -8 &&
               right_edge_line[down_num - i] - right_edge_line[down_num - i - 7] <= -8){
                temp_jump_point = (uint8)(down_num - i) + 3;
                return temp_jump_point;
            }
        }
    }
    else{
        temp_jump_point = up_num;
        for(int i = 0; i < down_num - up_num; i++){
            if(right_edge_line[up_num + i] - right_edge_line[up_num + i + 5] <= -8 &&
               right_edge_line[up_num + i] - right_edge_line[up_num + i + 6] <= -8 &&
               right_edge_line[up_num + i] - right_edge_line[up_num + i + 7] <= -8){
                temp_jump_point = (uint8)(up_num + i) - 3;
                return temp_jump_point;
            }
        }
    }
    return 0;
}

uint8 find_left_jump_point(uint8 down_num, uint8 up_num, uint8 model)
{
    uint8 temp_jump_point = 0;
    
    if(model){
        temp_jump_point = down_num;
        for(int i = 0; i < down_num - up_num; i++){
            if(left_edge_line[down_num - i] - left_edge_line[down_num - i - 5] >= 8 &&
               left_edge_line[down_num - i] - left_edge_line[down_num - i - 6] >= 8 &&
               left_edge_line[down_num - i] - left_edge_line[down_num - i - 7] >= 8){
                temp_jump_point = (uint8)(down_num - i) + 3;
                return temp_jump_point;
            }
        }
    }
    else{
        temp_jump_point = up_num;
        for(int i = 0; i < down_num - up_num; i++){
            if(left_edge_line[up_num + i] - left_edge_line[up_num + i + 5] >= 8 &&
               left_edge_line[up_num + i] - left_edge_line[up_num + i + 6] >= 8 &&
               left_edge_line[up_num + i] - left_edge_line[up_num + i + 7] >= 8){
                temp_jump_point = (uint8)(up_num + i) - 3;
                return temp_jump_point;
            }
        }
    }
    return 0;
}

void cross_analysis(void)
{
    uint32 track_width = 0;
    uint8 start_point = 0, end_point = 0;
    
    for(int i = (SEARCH_IMAGE_H * 2 / 3); i > (SEARCH_IMAGE_H / 3); i--)
		{
        track_width += (right_edge_line[i] - left_edge_line[i]);
    }
    
    if(!cross_flag && track_width > (SEARCH_IMAGE_W * (SEARCH_IMAGE_H * 4 / 15))){
        cross_flag = 1;
    }
    
    if(cross_flag == 1)
		{
        start_point = find_left_jump_point(SEARCH_IMAGE_H - 5, SEARCH_IMAGE_H/4, 0);
        end_point = find_left_jump_point(SEARCH_IMAGE_H - 5, SEARCH_IMAGE_H/3, 1);
        
        if(end_point && start_point){
            connect_point(left_edge_line, end_point, start_point);
        }
        if(end_point & !start_point){
            stretch_point(left_edge_line, end_point, 1);
        }
        if(!end_point && start_point){
            stretch_point(left_edge_line, start_point, 0);
        }
        
        start_point = find_right_jump_point(SEARCH_IMAGE_H - 5, SEARCH_IMAGE_H/4, 0);
        end_point = find_right_jump_point(SEARCH_IMAGE_H - 5, SEARCH_IMAGE_H/3, 1);
        
        if(end_point && start_point){
            connect_point(right_edge_line, end_point, start_point);
        }
        if(end_point && !start_point){
            stretch_point(right_edge_line, end_point, 1);
        }
        if(!end_point && start_point){
            stretch_point(right_edge_line, start_point, 0);
        }
        
        if(track_width < (SEARCH_IMAGE_W * (SEARCH_IMAGE_H * 1 / 5)))
				{
            cross_flag = 0;
        }
    }
}

void Connect_Circle_In()
{
	if(circle_flag == 1)
	{
		for(int i = 0;i < 120;i++)
		{
			left_edge_line[i] = 188 - 1.5 * i - 20;            //-20需要早点转,改前瞻或者第一个编码器,买没有测好/不减20会有一个明显摆头
		if(left_edge_line[i] > 188)left_edge_line[i] = 188;
		if(left_edge_line[i] < 0)left_edge_line[i] = 0;
		}
	}
	else if(circle_flag == -1)
	{
		for(int i = 0;i < 120;i++)
		{
			right_edge_line[i] = 1.5 * i + 20;
		if(right_edge_line[i] > 188)right_edge_line[i] = 188;
		if(right_edge_line[i] < 0)right_edge_line[i] = 0;
		}		
	}
}

void banmaxian_stop(const uint8_t *image)
{
	int16 temp1 = 0,temp2 = 0,temp3 = 0,count = 0;
	for(int i = 0; i < SEARCH_IMAGE_W - 3; i+=3)
	{
		for( int j = 79; j < 81; j++)
		{
			temp1 = *(image + j * SEARCH_IMAGE_W + i);
			temp2 = *(image + j * SEARCH_IMAGE_W + i + 3);
			temp3 = (temp1 - temp2)*200/(temp1 + temp2);
			if(temp3 > reference_contrast_ratio)count ++;
		}
	}
	if(count > 20)banmaxian_flag ++;
}
void Find_obstacle()
{
		for(int i = SEARCH_IMAGE_H-40;i>20;i--)
	  {
			if((right_edge_line[i + 1]-right_edge_line[i] > 5) || right_edge_line[i + 1] < right_edge_line[i])
			{			
				obstacle_flag = 10;
				return;
			}
		}
		for(int i = SEARCH_IMAGE_H-40;i>30;i--)
		{
			if(right_edge_line[i] > 183)
			{
				obstacle_flag = 20;
				return;
			}
		}	
//		uint16 temp = 0;
//		for(int i = 0; i < SEARCH_IMAGE_H;i++)
//		{
//			temp += center_line[i];
//		}
//		temp = temp / 120;
		uint16_t up_point = 0,down_point = 0;
		for(int i = 15; i < SEARCH_IMAGE_H - 30;i++)
		{
				if(
					left_edge_line[i - 3] < left_edge_line[i] &&
					left_edge_line[i - 5] < left_edge_line[i] )
				{
				up_point = i;
					break;
				}
		}
		for(int i = up_point + 5; i < SEARCH_IMAGE_H - 30;i++)
		{
			if(
					(left_edge_line[i] -  left_edge_line[i + 3] > 5) &&
					(left_edge_line[i] - left_edge_line[i + 5] > 5) )
			{
				down_point = i;
				break;
			}
		}
		if(up_point == 0 && down_point != 0)obstacle_flag = 30;
		if(up_point !=0 && down_point == 0)obstacle_flag = 40;
		if(up_point && down_point && (down_point - up_point < 20) )
		{
			obstacle_flag = 1;
		}
//		else obstacle_flag = 0;
	}
void straight_up()
{
		for(int i = SEARCH_IMAGE_H-20;i>20;i--)
	  {
			if(myabs(right_edge_line[i + 1]-right_edge_line[i]) > 5)
			{	
				speed_up = 0;				
				return;
			}
		}
		for(int i = SEARCH_IMAGE_H-20;i>30;i--)
		{
			if(right_edge_line[i] > 183)
			{
				speed_up = 0;		
				return;
			}
		}
		for(int i = SEARCH_IMAGE_H-20;i>20;i--)
	  {
			if(myabs(left_edge_line[i]-left_edge_line[i + 1]) > 5)
			{			
				speed_up = 0;						
				return;
			}
		}
		for(int i = SEARCH_IMAGE_H-20;i>30;i--)
		{
			if(left_edge_line[i] < 5)
			{
				speed_up = 0;						
				return;
			}
		}
		speed_up = 1;
		
}
void get_center_line()
{
	if(island_temp_flag == 2 || island_temp_flag == 4)
	{
		Connect_Circle_In();
	}
	if(center_line_mode == 1 && circle_flag == 1)       //寻右边线 右圆环
	{
		for(int i = 0;i < SEARCH_IMAGE_H;i++)
		{
			center_line[i]=right_edge_line[i] - half_line[i];
		}
	}
	else if(center_line_mode == 1 && circle_flag == 2)       //寻右边线 左圆环
	{
		for(int i = 0;i < SEARCH_IMAGE_H;i++)
		{
			center_line[i]=right_edge_line[i] - big_half_line[i];
		}
	}
	else if(center_line_mode == 2 && circle_flag == 1)         //寻左边线 右圆环
	{
		for(int i = 0;i < SEARCH_IMAGE_H;i++)
		{
			center_line[i]=left_edge_line[i] + big_half_line[i];
		}		
	}
	else if(center_line_mode == 2 && circle_flag == 2)         //寻左边线 左圆环
	{
		for(int i = 0;i < SEARCH_IMAGE_H;i++)
		{
			center_line[i]=left_edge_line[i] + half_line[i];
		}		
	}
	else if(center_line_mode == 0)                           //寻中线
	{
		for(int i = 0;i < SEARCH_IMAGE_H;i++)
		{
			center_line[i]=(left_edge_line[i] + right_edge_line[i])/2;
		}
	}
}