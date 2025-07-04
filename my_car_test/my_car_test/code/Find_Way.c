#include "mm32_device.h"                // Device header


int myabs(int num)
{
	if(num>=0)return num;
	else return (-num);
}

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

#define STRAIGHT 0
#define CROSS 1
#define ISLAND_IN 2
#define ISLAND_OUT 3

uint8_t Find_Cross(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 1;
}
uint8_t FindIsland_In(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 1;
}
uint8_t Find_Island_Out(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	return 1;
}

void Judging_Elements(uint8_t *arrary_value1,uint8_t *arrary_value2)
{
	uint8_t state	;
	switch (state) 
	{
        case STRAIGHT:
            if (Find_Cross(arrary_value1,arrary_value2))state = CROSS;
            else if (FindIsland_In(arrary_value1,arrary_value2)) state = ISLAND_IN;
            break;
        case CROSS:
            if (!Find_Cross(arrary_value1,arrary_value2)) state = STRAIGHT;
						else if(FindIsland_In(arrary_value1,arrary_value2)) state = ISLAND_IN;
            break;
        case ISLAND_IN:
					if(Find_Island_Out(arrary_value1,arrary_value2)) state = ISLAND_OUT;
            break;
        case ISLAND_OUT:
					if(!Find_Cross(arrary_value1,arrary_value2)) state = STRAIGHT;
            break;
    }
}


