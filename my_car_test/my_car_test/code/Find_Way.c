 #include "mm32_device.h"                // Device header
#include "search_line.h"
#include "find_way.h"
#include "my_func.h"

uint8_t element_name = 0;



uint8_t find_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1, uint8_t jump_num, uint8_t model)
{
	uint8_t temp_jump_point = 0;
	uint8_t temp_data;
	
	if(model)
	{
		temp_jump_point = num0;
		for(int i=0;i< (num0 - num1);i++)
		{
			temp_data = myabs(arrary_value[num0-i] - arrary_value[num1 - i - 1]);
			if(temp_data > jump_num)
			{
				temp_jump_point = (uint8_t)(num0 - i);
				break;
			}
		}
		
	}
	else
	{
		temp_jump_point = num1;
		for(int i = 0;i < (num1-num1);i++)
		{
			temp_data = myabs(arrary_value[num1 + i] - arrary_value[num1 + i +1]);
			if(temp_data > jump_num)
			{
				temp_jump_point = (uint8_t)(num0 + i);
				break;
			}
		}
	}
	return temp_jump_point;
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


uint8_t Find_Cross(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	uint8_t temp_flag = 0,temp_count = 0;
	for(int i = SEARCH_IMAGE_H - CROSSDOWNEDGE;i > (SEARCH_IMAGE_H - CROSSUPEDGE) ;i--)
	{
		if(arrary_value2[i] - arrary_value1[i] >= CROSSWIDTHTHRESHOLD )
		{
			if(temp_flag == 1)temp_count++;
				else temp_flag = 1;
		}
		else temp_flag = 0;
	}
	if(temp_count >= CROSSJUDGETHRESHOLD )return 1;
	else return 0;
}

uint8_t FindIsland_Ready(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 1;
}
uint8_t FindIsland_In(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 1;
}
uint8_t FindIsland_Ing(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 1;
}
uint8_t FindIsland_Out(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 1;
}
uint8_t Find_Island_Completed(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 1;
}

void Judging_Elements(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	switch (element_name) 
	{
        case STRAIGHT:
            if (Find_Cross(arrary_value1,arrary_value2))element_name = CROSS;
            else if (FindIsland_Ready(arrary_value1,arrary_value2)) element_name = ISLAND_READY;
						else element_name = STRAIGHT;
							break;
        case CROSS:
            if (!Find_Cross(arrary_value1,arrary_value2)) element_name = STRAIGHT;
						else element_name = CROSS;
							break;
        case ISLAND_READY:
					if(FindIsland_In(arrary_value1,arrary_value2)) element_name = ISLAND_IN;
					else element_name = ISLAND_READY;
            break;
				case ISLAND_IN:
					if(FindIsland_Ing(arrary_value1,arrary_value2)) element_name = ISLAND_ING;
					else element_name = ISLAND_IN;
						break;
				case ISLAND_ING:
					if(FindIsland_Out(arrary_value1,arrary_value2)) element_name = ISLAND_OUT;
					else element_name = ISLAND_ING;
						break;
        case ISLAND_OUT:
					if(Find_Island_Completed(arrary_value1,arrary_value2)) element_name = ISLAND_COMPLETED;
					else element_name = ISLAND_OUT;
            break;
				case ISLAND_COMPLETED:
					if(!Find_Island_Completed(arrary_value1,arrary_value2)) element_name = STRAIGHT;
					else element_name = ISLAND_COMPLETED;
    }
}

void Connect(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	int16_t start_point = 0,end_point = 0;
	if(element_name == CROSS)
	{
		start_point = find_jump_point(arrary_value1,CROSSUPEDGE,CROSSDOWNEDGE,30,1);
		end_point = find_jump_point(arrary_value1,CROSSUPEDGE,CROSSDOWNEDGE,30,0);
		connect_point(arrary_value1,start_point,end_point);
		start_point = find_jump_point(arrary_value2,CROSSUPEDGE,CROSSDOWNEDGE,30,1);
		end_point = find_jump_point(arrary_value2,CROSSUPEDGE,CROSSDOWNEDGE,30,0);
		connect_point(arrary_value2,start_point,end_point);		
	}
}

