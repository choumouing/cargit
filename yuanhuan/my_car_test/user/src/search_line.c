#include "zf_common_headfile.h"
#include "search_line.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ips200.h"
#include "Find_Way.h"
#include "my_func.h"

#define SEARCH_IMAGE_H  MT9V03X_H             // ( 120 )  
#define SEARCH_IMAGE_W  MT9V03X_W            //   ( 188 )                                         

#define REFRENCEROW      5              //参考点统计行数
#define REFERENCE_COL       80         //参考点统计列数
#define SEARCHRANGE     10         //搜线半径
#define STOPROW         0        //搜线停止行
#define CONTRASTOFFSET    3     //搜线对比偏移

#define BLACKPOINT  50         //黑点值
#define WHITEMAXMUL     13       // 白点最大值基于参考点的放大倍数  10为不放大
#define WHITEMINMUL       7        // 白点最小值基于参考点的放大倍数   10为不放大


uint8_t reference_point=0;         //动态参考点
uint8_t reference_col=0;          //动态参考列
uint8_t white_max_point=0;        //动态白点最大值
uint8_t white_min_point=0;        //动态白点最小值
uint8_t reference_contrast_ratio= 20;        //参考对比度
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


void Get_Reference_Point(const uint8_t *image) {
    uint32_t templ = 0;
    const int start_col = (SEARCH_IMAGE_W - REFERENCE_COL) / 2;
    const int end_col = (SEARCH_IMAGE_W + REFERENCE_COL) / 2;
    const int area = REFERENCE_ROW * (end_col - start_col);

    for (int i = SEARCH_IMAGE_H - REFERENCE_ROW; i < SEARCH_IMAGE_H; i++) {
        for (int j = start_col; j < end_col; j++) {
            templ += *(image + i * SEARCH_IMAGE_W + j);
        }
    }
    
    reference_point = templ / area;
    white_max_point = (uint8_t)func_limit_ab(
        (int32_t)reference_point * WHITEMAXMUL / 10, 
        BLACKPOINT, 
        255
    );
    white_min_point = (uint8_t)func_limit_ab(
        (int32_t)reference_point * WHITEMINMUL / 10, 
        BLACKPOINT, 
        255
    );
}

void Search_Reference_Col(const uint8_t *image) {
    for (int col = 0; col < SEARCH_IMAGE_W; col++) {
        remote_distance[col] = SEARCH_IMAGE_H - 1;
    }

    // 列搜索（步长CONTRASTOFFSET）
    for (int col = CONTRASTOFFSET; col < SEARCH_IMAGE_W; col += CONTRASTOFFSET) {
        for (int row = SEARCH_IMAGE_H - 1; row >= STOPROW + CONTRASTOFFSET; row -= CONTRASTOFFSET) {
            uint8_t temp1 = *(image + row * SEARCH_IMAGE_W + col);
            uint8_t temp2 = *(image + (row - CONTRASTOFFSET) * SEARCH_IMAGE_W + col);
            
            // 终止条件1：到达最小行
            if (row == STOPROW + CONTRASTOFFSET) {
                remote_distance[col] = row;
                break;
            }
            
            // 终止条件2：当前点过暗
            if (temp1 < white_min_point) {
                remote_distance[col] = row;
                break;
            }
            
            // 终止条件3：对比度跳变
            int16_t contrast = (temp1 - temp2) * 200 / (temp1 + temp2);
            if (contrast > reference_contrast_ratio) {
                remote_distance[col] = row;
                break;
            }
        }
    }
    
    // 找最远白列（最小行号）
    uint8_t min_row = remote_distance[0];
    reference_col = 0;
    for (int col = 0; col < SEARCH_IMAGE_W; col++) {
        if (remote_distance[col] < min_row) {
            min_row = remote_distance[col];
            reference_col = col;
        }
    }
}

void Search_Line(const uint8_t *image) {
    uint8_t *p;
    uint8_t row_max = SEARCH_IMAGE_H - 1;
    uint8_t row_min = STOPROW;
    uint8_t col_max = SEARCH_IMAGE_W - CONTRASTOFFSET - 1;
    uint8_t col_min = CONTRASTOFFSET;
    
    // 初始化边界
    for (int row = row_max; row >= row_min; row--) {
        left_edge_line[row] = col_min;
        right_edge_line[row] = col_max;
    }

    for (int row = row_max; row >= row_min; row--) {
        p = (uint8_t*)image + row * SEARCH_IMAGE_W;
        int left_search_time = 2;  // 左边界两次搜索机会
        int right_search_time = 2; // 右边界两次搜索机会
        int leftstop = 0, rightstop = 0;

        // ================== 左边界搜索 ==================
        do {
            int leftstartcol = (left_search_time == 2) ? reference_col : reference_col;
            int leftendcol = (left_search_time == 2) ? reference_col - SEARCHRANGE : col_min;
            leftendcol = (leftendcol < col_min) ? col_min : leftendcol;
            
            for (int col = leftstartcol; col >= leftendcol; col--) {
                // 列越界检查
                if (col - CONTRASTOFFSET < col_min) continue;
                
                uint8_t temp1 = *(p + col);
                uint8_t temp2 = *(p + col - CONTRASTOFFSET);
                
                // 情况1：起点即黑点（丢失边界）
                if (temp1 < white_min_point && col == reference_col && !leftstop) {
                    leftstop = 1;
                    for (int r = row; r >= row_min; r--) {
                        left_edge_line[r] = col_min;
                    }
                    left_search_time = 0;
                    break;
                }
                
                // 情况2：当前点为黑点
                if (temp1 < white_min_point) {
                    left_edge_line[row] = col;
                    left_search_time = 0;
                    break;
                }
                
                // 情况3：对比点过亮（跳过）
                if (temp2 > white_max_point) continue;
                
                // 情况4：对比度跳变检测
                int16_t contrast = (temp1 - temp2) * 200 / (temp1 + temp2 + 1);
                if (contrast > reference_contrast_ratio || col == col_min) {
                    left_edge_line[row] = (col - CONTRASTOFFSET < col_min) ? col_min : (col - CONTRASTOFFSET);
                    left_search_time = 0; // 成功找到边界
                    break;
                }
            }
            left_search_time--;
        } while (left_search_time > 0 && !leftstop);

        // ================== 右边界搜索 ==================
        do {
            int rightstartcol = (right_search_time == 2) ? reference_col : reference_col;
            int rightendcol = (right_search_time == 2) ? reference_col + SEARCHRANGE : col_max;
            rightendcol = (rightendcol > col_max) ? col_max : rightendcol;
            
            for (int col = rightstartcol; col <= rightendcol; col++) {
                // 列越界检查
                if (col + CONTRASTOFFSET > col_max) continue;
                
                uint8_t temp1 = *(p + col);
                uint8_t temp2 = *(p + col + CONTRASTOFFSET);
                
                // 情况1：起点即黑点（丢失边界）
                if (temp1 < white_min_point && col == reference_col && !rightstop) {
                    rightstop = 1;
                    for (int r = row; r >= row_min; r--) {
                        right_edge_line[r] = col_max;
                    }
                    right_search_time = 0;
                    break;
                }
                
                // 情况2：当前点为黑点
                if (temp1 < white_min_point) {
                    right_edge_line[row] = col;
                    right_search_time = 0;
                    break;
                }
                
                // 情况3：对比点过亮（跳过）
                if (temp2 > white_max_point) continue;
                
                // 情况4：对比度跳变检测
                int16_t contrast = (temp1 - temp2) * 200 / (temp1 + temp2 + 1);
                if (contrast > reference_contrast_ratio || col == col_max) {
                    right_edge_line[row] = (col + CONTRASTOFFSET > col_max) ? 
                                           col_max : (col + CONTRASTOFFSET);
                    right_search_time = 0; // 成功找到边界
                    break;
                }
            }
            right_search_time--;
        } while (right_search_time > 0 && !rightstop);
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
