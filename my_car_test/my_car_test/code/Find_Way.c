 #include "mm32_device.h"                // Device header
#include "search_line.h"
#include "find_way.h"
#include "my_func.h"

uint8_t element_name = 0;
int16_t test_data1 = 0,test_data2 = 0;


uint8_t find_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1, uint8_t jump_num, uint8_t model)
{
	uint8_t temp_jump_point = 0;
	uint8_t temp_data;
	
	if(model)
	{
		temp_jump_point = num0;
		for(int i=0;i< (num0 - num1);i++)
		{
			temp_data = myabs(arrary_value[num0-i] - arrary_value[num0 - i - 1]);
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
		for(int i = 0;i < (num0-num1);i++)
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


uint8_t Find_Cross_Ready(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	uint8_t temp_flag = 0,temp_count = 0;
	for(int i = SEARCH_IMAGE_H - CROSSDOWNEDGE;i > (SEARCH_IMAGE_H - CROSSUPEDGE) ;i--)
	{
		if(myabs(arrary_value2[i] - arrary_value1[i]) >= CROSSWIDTHTHRESHOLD )
		{
			if(temp_flag == 1)temp_count++;
				else temp_flag = 1;
		}
		else temp_flag = 0;
	}
	if(temp_count >= CROSSJUDGETHRESHOLD )return 1;
	else return 0;
}

uint8_t Find_Cross_In(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	uint8_t temp_left_flag = 0,temp_right_flag = 0;
	int16_t temp_left = 0,temp_right = 0;
	for(int i = 110;i > 80;i--)
	{
		temp_left += arrary_value1[i];
		temp_right += arrary_value2[i];
	}
	if(temp_left < (30 * 5) )temp_left_flag = 1;
	if(temp_right > (30 * 183) )temp_right_flag = 1;
	if(temp_left_flag && temp_right_flag)return 1;
	else return 0;
}

uint8_t FindIsland_Ready(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	uint8_t temp_count = 0,left_lianxu_flag = 0,right_island_flag = 0;
	uint8_t temp1 = 0,temp2 = 0;
	for(int i = 0;i < SEARCH_IMAGE_H - 10;i++)
	{
		if(myabs(arrary_value1[i] - arrary_value1[i+1]) >= LIANXUTHRESHOLD)temp_count ++;
	}
	if(temp_count <= 10)left_lianxu_flag = 1;
	temp1 = find_jump_point(arrary_value2,SEARCH_IMAGE_H - 10, 0 ,ISLANDJUMPTHRESHOLD,1);
	temp2 = find_jump_point(arrary_value2,SEARCH_IMAGE_H - 10, 0 ,ISLANDJUMPTHRESHOLD,0);
	if((temp1 - temp2) > ISLANDREADYJUDGETHRESHOLD)right_island_flag = 1;
	if(left_lianxu_flag && right_island_flag)return 0;
	else return 0;
}
uint8_t FindIsland_In(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 0;
}
uint8_t FindIsland_Ing(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 0;
}
uint8_t FindIsland_Out(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 0;
}
uint8_t Find_Island_Completed(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 0;
}

void Judging_Elements(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	switch (element_name) 
	{
        case STRAIGHT:
            if (Find_Cross_Ready(arrary_value1,arrary_value2))element_name = CROSS_READY;
            else if (FindIsland_Ready(arrary_value1,arrary_value2)) element_name = ISLAND_READY;
						else element_name = STRAIGHT;
							break;
        case CROSS_READY:
            if (Find_Cross_In(arrary_value1,arrary_value2)) element_name = CROSS_IN;
						else element_name = CROSS_READY;
							break;
				case CROSS_IN:
						if (!Find_Cross_In(arrary_value1,arrary_value2)) element_name = STRAIGHT;
						else element_name = CROSS_IN;
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
	float temp_slope = 0;
	if(element_name == CROSS_READY)
	{
		start_point = find_jump_point(arrary_value1,CROSSUPEDGE,CROSSDOWNEDGE,CROSSJUMPTHRESHOLD,1);
//		end_point = find_jump_point(arrary_value1,CROSSUPEDGE,CROSSDOWNEDGE,CROSSJUMPTHRESHOLD,0);	
		end_point = find_jump_point(arrary_value1,start_point,CROSSDOWNEDGE,CROSSJUMPTHRESHOLD,1);
		connect_point(arrary_value1,start_point,end_point - 10);
		
	temp_slope = ((float)arrary_value1[start_point] - (float)arrary_value1[end_point]) / (end_point - start_point);
	for(int i = 0;i < (start_point - end_point);i++)
	{
		arrary_value2[start_point - i] = (int8_t)((-temp_slope) * i) + arrary_value2[start_point];
		if(arrary_value2[start_point - i] >= 188)arrary_value2[start_point - i] = 188;
		if(arrary_value2[start_point - i] <= 0)arrary_value2[start_point - i] = 0;
	}
		start_point = find_jump_point(arrary_value2,CROSSUPEDGE,CROSSDOWNEDGE,CROSSJUMPTHRESHOLD,1);
//		end_point = find_jump_point(arrary_value2,CROSSUPEDGE,CROSSDOWNEDGE,CROSSJUMPTHRESHOLD,0);
		end_point = find_jump_point(arrary_value2,start_point,CROSSDOWNEDGE,CROSSJUMPTHRESHOLD,1);
		connect_point(arrary_value2,start_point,end_point - 10);		
	}
	else if(element_name == CROSS_IN)
	{
		start_point = 120;
		end_point = find_jump_point(arrary_value1,110,10,CROSSJUMPTHRESHOLD,0);
		temp_slope = ((float)arrary_value1[end_point] - (float)arrary_value1[end_point - 20]) / (-20);
		for(int i = 0;i < (start_point - end_point);i++)
		{
			arrary_value1[end_point + i] = (int8_t)((temp_slope) * i) + arrary_value1[end_point];
			if(arrary_value1[end_point + i] >= 188)arrary_value1[end_point + i] = 188;
			if(arrary_value1[end_point + i] <= 0)arrary_value1[end_point + i] = 0;
		}
		for(int i = 0;i < (start_point - end_point);i++)
		{
			arrary_value2[end_point + i] = -(int8_t)((temp_slope) * i) + arrary_value2[end_point];
			if(arrary_value2[end_point + i] >= 188)arrary_value2[end_point + i] = 188;
			if(arrary_value2[end_point + i] <= 0)arrary_value2[end_point + i] = 0;
		}
		
	}
}

