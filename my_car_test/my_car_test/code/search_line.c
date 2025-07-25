#include "zf_common_headfile.h"
#include "search_line.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ips200.h"
#include "Find_Way.h"
#include "my_func.h"

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


uint8_t reference_point=0;         //动态参考点
uint8_t reference_col=0;          //动态参考列
uint8_t white_max_point=0;        //动态白点最大值
uint8_t white_min_point=0;        //动态白点最小值
uint8_t reference_contrast_ratio= 15;        //参考对比度
uint8_t reference_col_line[SEARCH_IMAGE_H] ={0};//参考列绘制
uint8_t remote_distance[SEARCH_IMAGE_W]={0};          //白点远端距离
uint8_t left_edge_line[SEARCH_IMAGE_H]={0};          //左右边界
uint8_t right_edge_line[SEARCH_IMAGE_H]={0};

static uint8_t last_reference_col = SEARCH_IMAGE_W / 2; // 初始化为图像中间

uint8_t prospect = 20,top = 0;

int16_t weight1=0,weight2=0,weight3=0,weight4=0,weight5=0,weight6=0,weight7=0,weight8=0,weight9=0,weight10=0,weight11=0;
int16_t center_line_weight[11] = {0,1,1,3,5,9,9,9,5,1,0};
int16_t center_line_weight_buffer[11] = {0};
int32_t center_line_weight_temp = 0;
int32_t center_line_weight_count = 0;
int32_t center_line_weight_final = 0;

void Get_Reference_Point(const uint8_t *image)
	{
		uint8_t *p = (uint8_t *)&image[(SEARCH_IMAGE_H-REFRENCEROW)* SEARCH_IMAGE_W + REFRENCESTARTCOL];
		uint16_t  temp = 0;                        //保存统计点总数量

		uint32_t templ = 0;                       //保存所有统计点加起来的和

		temp = REFRENCEROW* (REFRENCEENDCOL - REFRENCESTARTCOL);      //计算待统计点总数量

		for(int i = 0; i< REFRENCEROW; i ++)         //统计点求和
		{
			for(int j = REFRENCESTARTCOL;j <= REFRENCEENDCOL;j++)
			{
			templ += * (p+(i * SEARCH_IMAGE_W) + j);
			}
		}

		reference_point = (uint8_t) (templ / temp);          //计算点平均值，作为本幅图像的参考点
	//	white_max_point=reference_point*WHITEMAXMUL/10;
	//	if(white_max_point>255)white_max_point=255;
	//	if(white_max_point<BLACKPOINT)white_max_point=BLACKPOINT;
	//	white_min_point=reference_point*WHITEMINMUL/10;	
	//	if(white_min_point>255)white_min_point=255;
	//	if(white_min_point<BLACKPOINT)white_min_point=BLACKPOINT;	
			white_max_point = 200;
			white_min_point = 120;
	}

void Search_Reference_Col(const uint8_t *image)
	{
		int col,row;
		int16 temp1=0,temp2=0,temp3=0;
		
		for(col=0;col<SEARCH_IMAGE_W;col++)
		{
			remote_distance[col]=SEARCH_IMAGE_H-1;
		}
		
		
		for(col = CONTRASTOFFSET; col <SEARCH_IMAGE_W; col += CONTRASTOFFSET)
		{
			for(row = SEARCH_IMAGE_H - CONTRASTOFFSET; row > CONTRASTOFFSET; row -= CONTRASTOFFSET)
			{
				temp1 = *(image + row *SEARCH_IMAGE_W + col);
				temp2 = *(image + (row - CONTRASTOFFSET) * SEARCH_IMAGE_W + col);
				if(temp2 > white_max_point)continue;
				if(temp1 < white_min_point)
				{
					remote_distance[col] = (uint8_t)row;
					break;
				}
				temp3 = (temp1 - temp2) * 200 / (temp1 + temp2 + 1);
				if(temp3>reference_contrast_ratio || row <= CONTRASTOFFSET)
				{
					remote_distance[col]=(uint8_t)row;
					break;
				}
			}

		}
		int32 temp_min=0,temp_min_index;
		temp_min=remote_distance[10];
		for(uint16 i=11;i<=(SEARCH_IMAGE_W-10);i++)
		{
			if(remote_distance[i]<temp_min)
			{
				temp_min=remote_distance[i];
				temp_min_index = i;
			}
		}
		uint8_t current_ref_col = temp_min_index + CONTRASTOFFSET;
		if(current_ref_col > 168 || current_ref_col < 20) 
		{
			reference_col = last_reference_col;
		} 
		else 
		{
			reference_col = current_ref_col;
			last_reference_col = current_ref_col; // 更新历史值
		}
		if(reference_col<1)reference_col=1;
		if(reference_col>(SEARCH_IMAGE_W-2))reference_col=SEARCH_IMAGE_W-2;
		
		for(int i=0;i<SEARCH_IMAGE_H;i++)
		{
			reference_col_line[i]=reference_col;
		}
		 
	}

void Search_Line(const uint8_t *image)          //搜索赛道边界
	{
		uint8_t *p = (uint8_t *)&image[0];            //图像数组指针
		uint8_t row_max = SEARCH_IMAGE_H - 1;      //行最大值
		uint8_t row_min = STOPROW;                 //行最小值
		uint8_t col_max = SEARCH_IMAGE_W - CONTRASTOFFSET;//列最大值
		uint8_t col_min = CONTRASTOFFSET;             //列最小值
		int16_t leftstartcol = reference_col;         //搜线左起始列
		int16_t rightstartcol = reference_col;        //搜线右起始列
		int16_t leftendcol=0;                         //搜线左终止列
		int16_t rightendcol = SEARCH_IMAGE_W -1;       //搜线右终止列
		uint8_t search_time=0;                       //单点搜线次数
		uint8_t temp1 = 0,temp2 = 0;                //临时变量     用于存储图像数据
		int temp3=0;                     //临时变量 用于存储对比度
		int leftstop = 0,rightstop = 0, stoppoint = 0;// 搜线自锁变量
		
		int col, row;

		for(row = row_max; row >= row_min; row --)
		{
		left_edge_line[row] = col_min- CONTRASTOFFSET;
		right_edge_line[row] = col_max + CONTRASTOFFSET;
		}
		for(row = row_max;row >= row_min;row--)
		{
			p=(uint8_t *)&image[row * SEARCH_IMAGE_W];       //获取本行起始点位置指针
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
						temp1=*(p + col);              //获取当前点灰度值
						temp2=*(p + col -CONTRASTOFFSET);        //获取对比点灰度值
						if(temp1 < white_min_point && col == leftstartcol && leftstartcol == reference_col)        //判断参考列是否为黑点
						{
							leftstop = 1;            //线搜索自锁，不进行左边线搜索
							for(stoppoint = row;stoppoint >=0 ;stoppoint--)
							{
								left_edge_line[stoppoint] = col_min;
							}
							search_time = 0;
							break;
						}
						if(temp1 < white_min_point)              //判断当前点是否为黑点
						{
							left_edge_line[row] = (uint8_t)col;
							break;
						}
						if(temp2 > white_max_point)              //判断当前点是否为白点
						{
							continue;
						}
						temp3 = (temp1 - temp2) * 200 / (temp1 + temp2 + 1);
						if(temp3 > reference_contrast_ratio || col == col_min)       //判断对比度是否到达阈值，或者已经到边界
						{
							left_edge_line[row] = col -CONTRASTOFFSET;          //保存当前行左边界
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
						temp1=*(p + col);              //获取当前点灰度值
						temp2=*(p + col + CONTRASTOFFSET);        //获取对比点灰度值
						if(temp1 < white_min_point && col == reference_col && rightstartcol == reference_col)        //判断参考列是否为黑点
						{
							rightstop = 1;            //线搜索自锁，不进行右边线搜索
							for(stoppoint = row;stoppoint >=0 ;stoppoint--)
							{
								right_edge_line[stoppoint] = col_max;
							}
							search_time = 0;
							break;
						}
						if(temp1 < white_min_point)              //判断当前点是否为黑点
						{
							right_edge_line[row] = (uint8_t)col;
							break;
						}
						if(temp2 > white_max_point)              //判断当前点是否为白点
						{
							continue;
						}
						temp3 = (temp1 - temp2) * 200 / (temp1 + temp2 + 1);
						
						if(temp3 > reference_contrast_ratio || col == col_max)       //判断对比度是否到达阈值，或者已经到边界
						{
							right_edge_line[row] = col + CONTRASTOFFSET;          //保存当前行右边界
							if(right_edge_line[row] < 0) right_edge_line[row] = 0;
							else if(right_edge_line[row] >= SEARCH_IMAGE_W) right_edge_line[row] = SEARCH_IMAGE_W - 1;
							rightstartcol = col - SEARCHRANGE;
							if(rightstartcol < col_min)rightstartcol = col_min;
							if(rightstartcol > col)rightstartcol = col;
							rightendcol=col + SEARCHRANGE;
							if(rightendcol < col)rightendcol = col;
							if(rightendcol > col_max)rightendcol = col_max;
							search_time = 0;
							break;
						 }
					}	
				}while(search_time);
			}
		}
	}

void image_calculate_prospect(const uint8_t *image)
{
	int col = SEARCH_IMAGE_W/2;
	int16 temp1 = 0,temp2 = 0,temp3 = 0;
	for	(int row = SEARCH_IMAGE_H-1;row > CONTRASTOFFSET;row--)
	{    
		temp1 = *(image+row* SEARCH_IMAGE_W + col );
		temp2 = *(image + (row-CONTRASTOFFSET) * SEARCH_IMAGE_W + col);
		
		if(row == 5){   //计算对比度
			prospect = SEARCH_IMAGE_H-(uint8)row;
			break;
		}
		
		if(temp1 < white_min_point){           //当前点是黑点
			prospect = SEARCH_IMAGE_H-(uint8)row;
			break;
		}
		
		if(temp2 > white_max_point){           //对比点是白点
			continue;
		}
		
		
		temp3 = (temp1 - temp2)*200/(temp1 + temp2);
		
		if(temp3 >reference_contrast_ratio ){   //计算对比度
			prospect = SEARCH_IMAGE_H-(uint8)row;
			break;
		}
	}
//	ips200_show_int(96,224,prospect,4);

}


void Find_Edge_At_Reference_Col(const uint8_t *image) 
{
    uint8_t row;  // 当前行
    uint8_t temp1, temp2;  // 灰度值临时变量
    int16_t temp3;  // 对比度计算结果
    
    // 从图像底部向上搜索到偏移量允许的最小行
    for (row = SEARCH_IMAGE_H - 1; row >= CONTRASTOFFSET; row--) 
    {
        // 获取当前点和上方偏移点的灰度值
        temp1 = *(image + row * SEARCH_IMAGE_W + reference_col);
        temp2 = *(image + (row - CONTRASTOFFSET) * SEARCH_IMAGE_W + reference_col);
        
        // 情况1：当前点是黑点（赛道内）
        if (temp1 < white_min_point) 
        {
            // 继续向上搜索（仍在赛道内）
            continue;
        }
        
        // 情况2：当前点是白点（背景）
        if (temp1 > white_max_point) 
        {
            // 直接返回当前行作为边界点
            top = row;
        }
        
        // 情况3：计算对比度
        temp3 = (temp1 - temp2) * 200 / (temp1 + temp2 + 1);  // +1防止除零
        
        // 满足对比度阈值条件
        if (temp3 > reference_contrast_ratio) 
        {
            top = row;  // 返回当前行作为边界点
        }
    }
}
