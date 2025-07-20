#ifndef __FIND_WAY_H__
#define __FIND_WAY_H__


#define LIANXUTHRESHOLD 5

#define CROSSDOWNEDGE 20
#define CROSSUPEDGE 100
#define CROSSWIDTHTHRESHOLD 170         //Ê®×Ö ÅÐ¶Ï¿í¶ÈãÐÖµ
#define CROSSJUDGETHRESHOLD 30					
#define CROSSJUMPTHRESHOLD 20           //Ê®×Ö ÅÐ¶ÏÌø±äãÐÖµ

#define ISLANDREADYJUDGETHRESHOLD 50
#define ISLANDJUMPTHRESHOLD 10           //ISLAND READY ÅÐ¶ÏÌø±äãÐÖµ

#define STRAIGHT 0
#define CROSS_READY 1
#define CROSS_IN 2
#define ISLAND_READY 3
#define ISLAND_IN 4
#define ISLAND_ING 5
#define ISLAND_OUT 6
#define ISLAND_COMPLETED 7

extern uint8_t element_name;
extern int16_t test_data1,test_data2,test_data3;
extern int16_t left_nomal_flag,right_nomal_flag;
extern int16_t island_temp_flag;
extern uint8_t left_half_line[SEARCH_IMAGE_H];
extern uint8_t half_line[SEARCH_IMAGE_H];
extern uint16 circle_time;
extern float circle_gyro_z;
extern float angle_temp;
extern int encoder_temp;

uint8_t find_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1, uint8_t jump_num, uint8_t model);
void connect_point(uint8_t *arrary_value,uint8_t num0,uint8_t num1);
uint8_t Find_Cross_Ready(uint8_t *arrary_value1,uint8_t *arrary_value2);
uint8_t Find_Cross_In(uint8_t *arrary_value1,uint8_t *arrary_value2);
void FindIsland_Ready();
void FinfIsland_In();
void FindIsland_Ing();
void FindIsland_Outready();
void FindIsland_Out();
void FindIsland_Complete();
void get_center_weight();
void Connect_Circle_In();
uint8 image_find_circle_point(uint8_t *edge_line,uint8 down_num,uint8 up_num,uint8 model);
void calculate_half_way();
//uint8_t find_circle_up_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1);
uint8_t find_circle_down_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1);

#endif
