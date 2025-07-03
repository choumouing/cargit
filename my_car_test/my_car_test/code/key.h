#ifndef _key_h
#define _key_h

#define KEY_1               E5                                                       // 定义主板上按键对应引脚
int  key1_can(void);

#define KEY_2               E4                                                       // 定义主板上按键对应引脚
int  key2_can(void);

#define KEY_3               E3                                                      // 定义主板上按键对应引脚
int  key3_can(void);

#define KEY_4               E2                                                       // 定义主板上按键对应引脚
int  key4_can(void);

#define LONG_PRESS_TIME	    30	//约为30*20*2=600ms
extern void key_into(void);
extern void button_entry(void *parameter);

#endif








