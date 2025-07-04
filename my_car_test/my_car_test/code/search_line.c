#include "zf_common_headfile.h"

#define SEARCH_IMAGE_H  MT9V03X_H             // ( 120 )  
#define SEARCH_IMAGE_W  MT9V03X_W            //   ( 188 )                                         



#define REFRENCEROW      5              //�ο���ͳ������
#define REFRENCESTARTCOL 64							//�ο���ͳ����ʼ��
#define REFRENCEENDCOL 124							//�ο���ͳ�ƽ�����
#define SEARCHRANGE     10         //���߰뾶
#define STOPROW         0        //����ֹͣ��
#define CONTRASTOFFSET    3     //���߶Ա�ƫ��

#define BLACKPOINT  50         //�ڵ�ֵ
#define WHITEMAXMUL     14       // �׵����ֵ���ڲο���ķŴ���  10Ϊ���Ŵ�
#define WHITEMINMUL       6        // �׵���Сֵ���ڲο���ķŴ���   10Ϊ���Ŵ�

uint8 reference_point=0;         //��̬�ο���
uint8 reference_col=0;          //��̬�ο���
uint8 white_max_point=0;        //��̬�׵����ֵ
uint8 white_min_point=0;        //��̬�׵���Сֵ
uint8 reference_contrast_ratio=15;        //�ο��Աȶ�
uint8 reference_col_line[SEARCH_IMAGE_H] ={0};//�ο��л���
uint8 remote_distance[SEARCH_IMAGE_W]={0};          //�׵�Զ�˾���
uint8 left_edge_line[SEARCH_IMAGE_H]={0};          //���ұ߽�
uint8 right_edge_line[SEARCH_IMAGE_H]={0};
uint32 if_count=0;


void Get_Reference_Point(const uint8 *image)
{
	uint8 *p = (uint8 *)&image[(SEARCH_IMAGE_H-REFRENCEROW)* SEARCH_IMAGE_W + REFRENCESTARTCOL];
	uint16  temp = 0;                        //����ͳ�Ƶ�������

	uint32 templ = 0;                       //��������ͳ�Ƶ�������ĺ�

	temp = REFRENCEROW* (REFRENCEENDCOL - REFRENCESTARTCOL);      //�����ͳ�Ƶ�������

	for(int i = 0; i< REFRENCEROW; i ++)         //ͳ�Ƶ����
	{
		for(int j = REFRENCESTARTCOL;j <= REFRENCEENDCOL;j++)
		{
		templ += * (p+(i * SEARCH_IMAGE_W) + j);
		}
	}

	reference_point = (uint8) (templ / temp);          //�����ƽ��ֵ����Ϊ����ͼ��Ĳο���
	white_max_point=reference_point*WHITEMAXMUL/10;
	if(white_max_point>255)white_max_point=255;
	if(white_max_point<BLACKPOINT)white_max_point=BLACKPOINT;
	white_min_point=reference_point*WHITEMINMUL/10;	
	if(white_min_point>255)white_max_point=255;
	if(white_min_point<BLACKPOINT)white_max_point=BLACKPOINT;	

}

void Search_Reference_Col(const uint8 *image)
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
				remote_distance[col] = (uint8)row;
				break;
			}
			temp3 = (temp1 - temp2) * 200 / (temp1 + temp2);
			if(temp3>reference_contrast_ratio || row==STOPROW)
			{
				remote_distance[col]=(uint8)row;
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

void Search_Line(const uint8 *image)          //���������߽�
{
	uint8 *p = (uint8 *)&image[0];            //ͼ������ָ��
	uint8 row_max = SEARCH_IMAGE_H - 1;      //�����ֵ
  uint8 row_min = STOPROW;                 //����Сֵ
  uint8 col_max = SEARCH_IMAGE_W - CONTRASTOFFSET;//�����ֵ
	uint8 col_min = CONTRASTOFFSET;             //����Сֵ
  int16 leftstartcol = reference_col;         //��������ʼ��
  int16 rightstartcol = reference_col;        //��������ʼ��
	int16 leftendcol=0;                         //��������ֹ��
	int16 rightendcol = SEARCH_IMAGE_W -1;       //��������ֹ��
	uint8 search_time=0;                       //�������ߴ���
	uint8 temp1 = 0,temp2 = 0;                //��ʱ����     ���ڴ洢ͼ������
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
		p=(uint8 *)&image[row * SEARCH_IMAGE_W];       //��ȡ������ʼ��λ��ָ��
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
						left_edge_line[row] = (uint8)col;
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
						right_edge_line[row] = (uint8)col;
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