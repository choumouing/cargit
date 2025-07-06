#include "zf_common_headfile.h"
#include "search_line.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ips200.h"
#include "Find_Way.h"

#define SEARCH_IMAGE_H  MT9V03X_H             // ( 120 )  
#define SEARCH_IMAGE_W  MT9V03X_W            //   ( 188 )                                         


uint8_t reference_point=0;         //��̬�ο���
uint8_t reference_col=0;          //��̬�ο���
uint8_t white_max_point=0;        //��̬�׵����ֵ
uint8_t white_min_point=0;        //��̬�׵���Сֵ
uint8_t reference_contrast_ratio=15;        //�ο��Աȶ�
uint8_t reference_col_line[SEARCH_IMAGE_H] ={0};//�ο��л���
uint8_t remote_distance[SEARCH_IMAGE_W]={0};          //�׵�Զ�˾���
uint8_t left_edge_line[SEARCH_IMAGE_H]={0};          //���ұ߽�
uint8_t right_edge_line[SEARCH_IMAGE_H]={0};
uint8_t center_line[SEARCH_IMAGE_H]={0};


void Get_Reference_Point(const uint8_t *image)
{
	uint8_t *p = (uint8_t *)&image[(SEARCH_IMAGE_H-REFRENCEROW)* SEARCH_IMAGE_W + REFRENCESTARTCOL];
	uint16_t  temp = 0;                        //����ͳ�Ƶ�������

	uint32_t templ = 0;                       //��������ͳ�Ƶ�������ĺ�

	temp = REFRENCEROW* (REFRENCEENDCOL - REFRENCESTARTCOL);      //�����ͳ�Ƶ�������

	for(int i = 0; i< REFRENCEROW; i ++)         //ͳ�Ƶ����
	{
		for(int j = REFRENCESTARTCOL;j <= REFRENCEENDCOL;j++)
		{
		templ += * (p+(i * SEARCH_IMAGE_W) + j);
		}
	}

	reference_point = (uint8_t) (templ / temp);          //�����ƽ��ֵ����Ϊ����ͼ��Ĳο���
	white_max_point=reference_point*WHITEMAXMUL/10;
	if(white_max_point>255)white_max_point=255;
	if(white_max_point<BLACKPOINT)white_max_point=BLACKPOINT;
	white_min_point=reference_point*WHITEMINMUL/10;	
	if(white_min_point>255)white_max_point=255;
	if(white_min_point<BLACKPOINT)white_max_point=BLACKPOINT;	

}

void Search_Reference_Col(const uint8_t *image)
{
	int col,row;
	int16 temp1=0,temp2=0,temp3=0;
	
	for(col=0;col<SEARCH_IMAGE_W;col++)
	{
		remote_distance[col]=SEARCH_IMAGE_H-1;
	}
	
	
	for(col = 0; col <SEARCH_IMAGE_W; col += CONTRASTOFFSET)
	{
		for(row = SEARCH_IMAGE_H - 1; row > STOPROW; row -= CONTRASTOFFSET)
		{
			temp1 = *(image + row *SEARCH_IMAGE_W + col);
			temp2 = *(image + (row - STOPROW) * SEARCH_IMAGE_W + col);
			if(temp2 > white_max_point)continue;
			if(temp1 < white_min_point)
			{
				remote_distance[col] = (uint8_t)row;
				break;
			}
			temp3 = (temp1 - temp2) * 200 / (temp1 + temp2);
			if(temp3>reference_contrast_ratio || row==STOPROW)
			{
				remote_distance[col]=(uint8_t)row;
				break;
			}
	  }

	}
	int32 temp_min=0;
	temp_min=remote_distance[10];
	for(uint16 i=10;i<=(SEARCH_IMAGE_W-10);i++)
	{
		if(remote_distance[i]<temp_min)temp_min=remote_distance[i];
	}
	reference_col=temp_min+CONTRASTOFFSET;
	if(reference_col<1)reference_col=1;
	if(reference_col>(SEARCH_IMAGE_W-2))reference_col=SEARCH_IMAGE_W-2;
	for(int i=0;i<SEARCH_IMAGE_H;i++)
	{
		reference_col_line[i]=reference_col;
	}
	
}

void Search_Line(const uint8_t *image)          //���������߽�
{
	uint8_t *p = (uint8_t *)&image[0];            //ͼ������ָ��
	uint8_t row_max = SEARCH_IMAGE_H - 1;      //�����ֵ
  uint8_t row_min = STOPROW;                 //����Сֵ
  uint8_t col_max = SEARCH_IMAGE_W - CONTRASTOFFSET;//�����ֵ
	uint8_t col_min = CONTRASTOFFSET;             //����Сֵ
  int16_t leftstartcol = reference_col;         //��������ʼ��
  int16_t rightstartcol = reference_col;        //��������ʼ��
	int16_t leftendcol=0;                         //��������ֹ��
	int16_t rightendcol = SEARCH_IMAGE_W -1;       //��������ֹ��
	uint8_t search_time=0;                       //�������ߴ���
	uint8_t temp1 = 0,temp2 = 0;                //��ʱ����     ���ڴ洢ͼ������
	int temp3=0;                     //��ʱ���� ���ڴ洢�Աȶ�
	int leftstop = 0,rightstop = 0, stoppoint = 0;// ������������
	
	int col, row;

	for(row = row_max; row >= row_min; row --)
	{
	left_edge_line[row] = col_min- CONTRASTOFFSET;
	right_edge_line[row] = col_max + CONTRASTOFFSET;
	}
	for(row = row_max;row >= row_min;row--)
	{
		p=(uint8_t *)&image[row * SEARCH_IMAGE_W];       //��ȡ������ʼ��λ��ָ��
		if(!leftstop)
		{
			search_time = 2;
			do
			{
				if(search_time == 1)
				{
					leftstartcol = reference_col;
					leftendcol =col_min;				
				}
				search_time --;
				for(col = leftstartcol;col >= leftendcol;col--)
				{
					temp1=*(p + col);              //��ȡ��ǰ��Ҷ�ֵ
					temp2=*(p + col -CONTRASTOFFSET);        //��ȡ�Աȵ�Ҷ�ֵ
					if(temp1 < white_min_point && col == leftendcol && leftstartcol == reference_col)        //�жϲο����Ƿ�Ϊ�ڵ�
					{
						leftstop = 1;            //���������������������������
						for(stoppoint = row;stoppoint >=0 ;stoppoint--)
						{
							left_edge_line[stoppoint] = col_min;
						}
						search_time = 0;
						break;
					}
					if(temp1 < white_min_point)              //�жϵ�ǰ���Ƿ�Ϊ�ڵ�
					{
						left_edge_line[row] = (uint8_t)col;
						break;
					}
					if(temp2 > white_max_point)              //�жϵ�ǰ���Ƿ�Ϊ�׵�
					{
						continue;
					}
					temp3 = (temp1 - temp2) * 200 / (temp1 + temp2);
					if(temp3 > reference_contrast_ratio || col == col_min)       //�ж϶Աȶ��Ƿ񵽴���ֵ�������Ѿ����߽�
					{
						left_edge_line[row] = col -CONTRASTOFFSET;          //���浱ǰ����߽�
						if(left_edge_line[row] < 0) left_edge_line[row] = 0;
						else if(left_edge_line[row] >= SEARCH_IMAGE_W) left_edge_line[row] = SEARCH_IMAGE_W - 1;
						leftstartcol = col + SEARCHRANGE;
						if(leftstartcol < col)leftstartcol = col;
						if(leftstartcol > col_max)leftstartcol = col_max;
						leftendcol=col - SEARCHRANGE;
						if(leftendcol < col_min)leftendcol = col_min;
						if(leftendcol > col)leftendcol = col;
						search_time = 0;
						break;
					 }
				 }	
			}while(search_time);
	}
	if(!rightstop)
	{
			search_time = 2;
			do
			{
				if(search_time == 1)
				{
					rightstartcol = reference_col;
					rightendcol = col_max;				
				}
				search_time --;
				for(col = rightstartcol;col <= rightendcol;col++)
				{
					temp1=*(p + col);              //��ȡ��ǰ��Ҷ�ֵ
					temp2=*(p + col + CONTRASTOFFSET);        //��ȡ�Աȵ�Ҷ�ֵ
					if(temp1 < white_min_point && col == rightendcol && rightstartcol == reference_col)        //�жϲο����Ƿ�Ϊ�ڵ�
					{
						rightstop = 1;            //�������������������ұ�������
						for(stoppoint = row;stoppoint >=0 ;stoppoint--)
						{
							right_edge_line[stoppoint] = col_max;
						}
						search_time = 0;
						break;
					}
					if(temp1 < white_min_point)              //�жϵ�ǰ���Ƿ�Ϊ�ڵ�
					{
						right_edge_line[row] = (uint8_t)col;
						break;
					}
					if(temp2 > white_max_point)              //�жϵ�ǰ���Ƿ�Ϊ�׵�
					{
						continue;
					}
					temp3 = (temp1 - temp2) * 200 / (temp1 + temp2);
					
					if(temp3 > reference_contrast_ratio || col == col_max)       //�ж϶Աȶ��Ƿ񵽴���ֵ�������Ѿ����߽�
					{
						right_edge_line[row] = col + CONTRASTOFFSET;          //���浱ǰ���ұ߽�
						if(right_edge_line[row] < 0) right_edge_line[row] = 0;
						else if(right_edge_line[row] >= SEARCH_IMAGE_W) right_edge_line[row] = SEARCH_IMAGE_W - 1;
						rightstartcol = col - SEARCHRANGE;
						if(rightstartcol < col)rightstartcol = col_min;
						if(rightstartcol > col_max)rightstartcol = col;
						leftendcol=col + SEARCHRANGE;
						if(rightendcol < col_min)rightendcol = col;
						if(rightendcol > col)rightendcol = col_max;
						search_time = 0;
						break;
					 }
				}	
			}while(search_time);
		}
	}
}

