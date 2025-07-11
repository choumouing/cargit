#ifndef __MY_FUNC_H__
#define __MY_FUNC_H__

extern uint8_t center_line[SEARCH_IMAGE_H];
extern uint16_t left_variance,right_variance;

int myabs(int num);

void Update_Line(const uint8_t *image);
void if_mt9v03x_init();							  //…„œÒÕ∑≥ı ºªØ
void ips_show_mt9v03x(uint8_t *image_buffer);
int16_t variance(uint8_t *arrary_value,uint8_t num1,uint8_t num2);
uint16_t calculate_variance(uint8_t array[], int size); 

#endif
