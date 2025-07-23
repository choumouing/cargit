#ifndef __FIND_WAY_H__
#define __FIND_WAY_H__


#define LIANXUTHRESHOLD 5

#define CROSSDOWNEDGE 20
#define CROSSUPEDGE 100
#define CROSSWIDTHTHRESHOLD 170         //十字 判断宽度阈值
#define CROSSJUDGETHRESHOLD 30					
#define CROSSJUMPTHRESHOLD 20           //十字 判断跳变阈值

#define ISLANDREADYJUDGETHRESHOLD 50
#define ISLANDJUMPTHRESHOLD 10           //ISLAND READY 判断跳变阈值

#define STRETCH_NUM       80          //补线延伸长度


extern int circle_flag;
extern uint8_t element_name;
extern int16_t test_data1,test_data2,test_data3;
extern int16_t left_nomal_flag,right_nomal_flag;
extern int16_t island_temp_flag;
extern uint8_t big_half_line[SEARCH_IMAGE_H];
extern uint8_t half_line[SEARCH_IMAGE_H];
extern uint16 circle_time;
extern float circle_gyro_z;
extern float angle_temp;
extern int encoder_temp_left,encoder_temp_right;
extern uint8 cross_flag;
extern uint8 banmaxian_flag;

uint8_t find_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1, uint8_t jump_num, uint8_t model);
void connect_point(uint8_t *arrary_value,uint8_t num0,uint8_t num1);
uint8_t Find_Cross_Ready(uint8_t *arrary_value1,uint8_t *arrary_value2);
uint8_t Find_Cross_In(uint8_t *arrary_value1,uint8_t *arrary_value2);
void FindIsland_Ready_Right();
void FindIsland_Ready_Left();
void FinfIsland_In_Right();
void FinfIsland_In_Left();
void FindIsland_Ing_Right();
void FindIsland_Ing_Left();
void FindIsland_Outready_Right();
void FindIsland_Outready_Left();
void FindIsland_Out_Right();
void FindIsland_Out_Left();
void FindIsland_Complete_Right();
void FindIsland_Complete_Left();
void get_center_weight();
void Connect_Circle_In();
void circle_state();
uint8 find_circle_point_right(uint8_t *edge_line,uint8 down_num,uint8 up_num,uint8 model);
//void calculate_half_way();
uint8_t find_circle_down_jump_point_right(uint8_t *arrary_value, uint8_t num0, uint8_t num1);
uint8_t find_circle_down_jump_point_left(uint8_t *arrary_value, uint8_t num0, uint8_t num1);
void banmaxian_stop(const uint8_t *image);
#endif
