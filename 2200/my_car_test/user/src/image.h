#ifndef _IMAGE_H
#define _IMAGE_H

#include "zf_common_headfile.h"

//宏定义
#define image_h 128//图像高度
#define image_w 160//图像宽度
#define dis_image_h 75//显示图像高度
#define dis_image_w 100//显示图像宽度         96*160



#define border_max  image_w-2 //边界最大值
#define border_min  1   //边界最小值

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128
#define HEART_SIZE 30
//定义膨胀和腐蚀的阈值区间
#define threshold_max   255*5//此参数可根据自己的需求调节
#define threshold_min   255*2//此参数可根据自己的需求调节
extern uint8_t cir_x,cir_y;
extern float cir_k1 ,cir_k2 ;
void cir_point(int flag);
#define USE_num image_h*10  //定义找点的数组成员个数按理说300个点能放下，但是有些特殊情况确实难顶，多定义了一点
float cir_k(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);
int my_abs(int value);
float my_abs2(float value);
uint8 Ctrl_x(uint8 data);
uint8 Ctrl_y(uint8 data);
uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row);
void width_change(void);
void turn_to_bin(void);
extern int16_t length;
uint8 get_start_point(uint8 star_row);
void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest);
void R_border(uint16 total_R);
void L_border(uint16 total_L);
void image_filter(uint8(*bin_image)[image_w]);//形态学滤波，简单来说就是膨胀和腐蚀的思想
void image_draw_rectan(uint8(*image)[image_w]);
void image_process(void);
uint8_t l_l_judge(void);//十字丢线判断
//void stop_line(void);

float control_kk(float k);
bool Find_stop(void);//该停车时返回1
bool line_stop(void);//该停车时返回1
float Find_Error(void);//计算图像测得的中线位置和图像中心直线的误差
float Find_hengyi_error(void);//计算图像下方10个点与中点的误差
float Process_Curvity(uint8 x1, uint8 y1, uint8 x2, uint8 y2, uint8 x3, uint8 y3);
void Element_test(void);
u8 straight_judge(void);
u8 straight_judge_2(void);
void Element_handle(void);
u8 cross_judge(void);
uint8 My_Sqrt(int16 x);
void draw_cir1(uint8 (*screen)[SCREEN_WIDTH]);
void draw_cir2(uint8 (*screen)[SCREEN_WIDTH]);
void center_valid(void);
void draw_cross(uint8 (*screen)[SCREEN_WIDTH]);
bool stop_line(void);
int cross_delay_judge(void);
void judge_bar(void);
extern uint8_t flag_r_cir;  //圆环状态清零
extern uint8_t flag_l_cir;  //圆环状态清零
extern uint8 flag_cross;//全局标志位
extern int8 flag_l,flag_r;
extern uint8_t cir_stage;
extern uint8_t Cir_hengyi_Flag;//0正常，1左圆环，2右圆环
extern uint8 left_right_flag;//判断是不是弯道，0直线，1左弯，2右弯
extern int16 Black_jiexian;//检测该图像上处是否存在全黑情况，并且得到全黑界限的纵坐标
extern uint8_t l_dir_sum[9];
extern uint8_t r_dir_sum[9];
extern int16 right_bor[image_h];
extern int16 left_bor[image_h];
extern int16 l_line_qulv;
extern int16 r_line_qulv;
extern int16_t r_lose_num ;
extern int16_t l_lose_num ;

extern uint8_t flag_others2;//用来区别直线加速 1普通pid 0元素
extern bool flag_str;//用来区别直线加速 1直线 0不是
extern bool flag_str_str;//用来区别直线

extern int count_line;
extern bool flag_stop ;
extern u8 flag_bar ;  // 1 为左侧障碍，2为右侧障碍
extern u8 count_l ,count_r;
extern int16 encoder_sum_l4,encoder_sum_l7,encoder_sum_r4,encoder_sum_r7;
extern int stop_line_time;
extern bool flag_time_stop;

extern int cross_jud;
#endif /*_IMAGE_H*/
