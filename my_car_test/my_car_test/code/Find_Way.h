#ifndef __FIND_WAY_H__
#define __FIND_WAY_H__



#define CROSSDOWNEDGE 20
#define CROSSUPEDGE 100
#define CROSSWIDTHTHRESHOLD 150
#define CROSSJUDGETHRESHOLD 30

#define STRAIGHT 0
#define CROSS 1
#define ISLAND_READY 2
#define ISLAND_IN 3
#define ISLAND_ING 4
#define ISLAND_OUT 5
#define ISLAND_COMPLETED 6

extern uint8_t element_name;



uint8_t find_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1, uint8_t jump_num, uint8_t model);
void connect_point(uint8_t *arrary_value,uint8_t num0,uint8_t num1);
uint8_t Find_Cross(uint8_t *arrary_value1,uint8_t *arrary_value2);
uint8_t FindIsland_In(uint8_t *arrary_value1,uint8_t *arrary_value2);
uint8_t Find_Island_Out(uint8_t *arrary_value1,uint8_t *arrary_value2);
void	Judging_Elements(uint8_t *arrary_value1,uint8_t *arrary_value2);

void Connect(uint8_t *arrary_value1,uint8_t *arrary_value2);



#endif
