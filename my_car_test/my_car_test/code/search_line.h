#ifndef __search_line_h
#define __search_line_h

#include "zf_device_mt9v03x.h"

#define SEARCH_IMAGE_H  MT9V03X_H             // ( 120 )  
#define SEARCH_IMAGE_W  MT9V03X_W            //   ( 188 )                                         

#define REFRENCEROW      5              //�ο���ͳ������
#define REFRENCESTARTCOL 64							//�ο���ͳ����ʼ��
#define REFRENCEENDCOL 124							//�ο���ͳ�ƽ�����
#define SEARCHRANGE     0         //���߰뾶
#define STOPROW         0        //����ֹͣ��
#define CONTRASTOFFSET    3     //���߶Ա�ƫ��

#define BLACKPOINT  50         //�ڵ�ֵ
#define WHITEMAXMUL     15       // �׵����ֵ���ڲο���ķŴ���  10Ϊ���Ŵ�
#define WHITEMINMUL       5        // �׵���Сֵ���ڲο���ķŴ���   10Ϊ���Ŵ�

extern uint8_t reference_point;         //��̬�ο���
extern uint8_t reference_col;          //��̬�ο���
extern uint8_t white_max_point;        //��̬�׵����ֵ
extern uint8_t white_min_point;        //��̬�׵���Сֵ
extern uint8_t reference_contrast_ratio;        //�ο��Աȶ�
extern uint8_t reference_col_line[SEARCH_IMAGE_H];//�ο��л���
extern uint8_t remote_distance[SEARCH_IMAGE_W];          //�׵�Զ�˾���
extern uint8_t left_edge_line[SEARCH_IMAGE_H];          //���ұ߽�
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
void Search_Line(const uint8_t *image);          //���������߽�
void image_calculate_prospect(const uint8_t *image);
void Find_Edge_At_Reference_Col(const uint8_t *image); 

#endif