#ifndef __search_line_h
#define __search_line_h

#include "zf_device_mt9v03x.h"

#define SEARCH_IMAGE_H  MT9V03X_H             // ( 120 )  
#define SEARCH_IMAGE_W  MT9V03X_W            //   ( 188 )                                         

#define REFRENCEROW      5              //参考点统计行数
#define REFRENCESTARTCOL 64							//参考点统计起始列
#define REFRENCEENDCOL 124							//参考点统计结束列
#define SEARCHRANGE     0         //搜线半径
#define STOPROW         0        //搜线停止行
#define CONTRASTOFFSET    3     //搜线对比偏移

#define BLACKPOINT  50         //黑点值
#define WHITEMAXMUL     15       // 白点最大值基于参考点的放大倍数  10为不放大
#define WHITEMINMUL       5        // 白点最小值基于参考点的放大倍数   10为不放大

extern uint8_t reference_point;         //动态参考点
extern uint8_t reference_col;          //动态参考列
extern uint8_t white_max_point;        //动态白点最大值
extern uint8_t white_min_point;        //动态白点最小值
extern uint8_t reference_contrast_ratio;        //参考对比度
extern uint8_t reference_col_line[SEARCH_IMAGE_H];//参考列绘制
extern uint8_t remote_distance[SEARCH_IMAGE_W];          //白点远端距离
extern uint8_t left_edge_line[SEARCH_IMAGE_H];          //左右边界
extern uint8_t right_edge_line[SEARCH_IMAGE_H];

extern uint32_t if_count;

extern char Image_Ready;
extern uint8_t prospect,top;

extern int16_t weight1,weight2,weight3,weight4,weight5,weight6,weight7,weight8,weight9,weight10,weight11;
extern int16_t center_line_weight[11];
extern int16_t center_line_weight_buffer[11];
extern int32_t center_line_weight_temp;
extern int32_t center_line_weight_count;
extern int32_t center_line_weight_final;

void Get_Reference_Point(const uint8_t *image);
void Search_Reference_Col(const uint8_t *image);
void Search_Line(const uint8_t *image);          //搜索赛道边界
void image_calculate_prospect(const uint8_t *image);
void Find_Edge_At_Reference_Col(const uint8_t *image); 

#endif