#ifndef __MY_FUNC_H__
#define __MY_FUNC_H__

extern uint8_t center_line[SEARCH_IMAGE_H];

int myabs(int num);

void Update_Line(const uint8_t *image);
void if_mt9v03x_init();							  //…„œÒÕ∑≥ı ºªØ
void ips_show_mt9v03x(uint8_t *image_buffer);


#endif
