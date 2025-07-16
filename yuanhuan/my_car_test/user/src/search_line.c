#include "zf_common_headfile.h"
#include "search_line.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ips200.h"
#include "Find_Way.h"
#include "my_func.h"

#define SEARCH_IMAGE_H  MT9V03X_H             // ( 120 )  
#define SEARCH_IMAGE_W  MT9V03X_W            //   ( 188 )                                         

#define REFRENCEROW      5              //�ο���ͳ������
#define REFERENCE_COL       80         //�ο���ͳ������
#define SEARCHRANGE     10         //���߰뾶
#define STOPROW         0        //����ֹͣ��
#define CONTRASTOFFSET    3     //���߶Ա�ƫ��

#define BLACKPOINT  50         //�ڵ�ֵ
#define WHITEMAXMUL     13       // �׵����ֵ���ڲο���ķŴ���  10Ϊ���Ŵ�
#define WHITEMINMUL       7        // �׵���Сֵ���ڲο���ķŴ���   10Ϊ���Ŵ�


uint8_t reference_point=0;         //��̬�ο���
uint8_t reference_col=0;          //��̬�ο���
uint8_t white_max_point=0;        //��̬�׵����ֵ
uint8_t white_min_point=0;        //��̬�׵���Сֵ
uint8_t reference_contrast_ratio= 20;        //�ο��Աȶ�
uint8_t reference_col_line[SEARCH_IMAGE_H] ={0};//�ο��л���
uint8_t remote_distance[SEARCH_IMAGE_W]={0};          //�׵�Զ�˾���
uint8_t left_edge_line[SEARCH_IMAGE_H]={0};          //���ұ߽�
uint8_t right_edge_line[SEARCH_IMAGE_H]={0};

static uint8_t last_reference_col = SEARCH_IMAGE_W / 2; // ��ʼ��Ϊͼ���м�

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

    // ������������CONTRASTOFFSET��
    for (int col = CONTRASTOFFSET; col < SEARCH_IMAGE_W; col += CONTRASTOFFSET) {
        for (int row = SEARCH_IMAGE_H - 1; row >= STOPROW + CONTRASTOFFSET; row -= CONTRASTOFFSET) {
            uint8_t temp1 = *(image + row * SEARCH_IMAGE_W + col);
            uint8_t temp2 = *(image + (row - CONTRASTOFFSET) * SEARCH_IMAGE_W + col);
            
            // ��ֹ����1��������С��
            if (row == STOPROW + CONTRASTOFFSET) {
                remote_distance[col] = row;
                break;
            }
            
            // ��ֹ����2����ǰ�����
            if (temp1 < white_min_point) {
                remote_distance[col] = row;
                break;
            }
            
            // ��ֹ����3���Աȶ�����
            int16_t contrast = (temp1 - temp2) * 200 / (temp1 + temp2);
            if (contrast > reference_contrast_ratio) {
                remote_distance[col] = row;
                break;
            }
        }
    }
    
    // ����Զ���У���С�кţ�
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
    
    // ��ʼ���߽�
    for (int row = row_max; row >= row_min; row--) {
        left_edge_line[row] = col_min;
        right_edge_line[row] = col_max;
    }

    for (int row = row_max; row >= row_min; row--) {
        p = (uint8_t*)image + row * SEARCH_IMAGE_W;
        int left_search_time = 2;  // ��߽�������������
        int right_search_time = 2; // �ұ߽�������������
        int leftstop = 0, rightstop = 0;

        // ================== ��߽����� ==================
        do {
            int leftstartcol = (left_search_time == 2) ? reference_col : reference_col;
            int leftendcol = (left_search_time == 2) ? reference_col - SEARCHRANGE : col_min;
            leftendcol = (leftendcol < col_min) ? col_min : leftendcol;
            
            for (int col = leftstartcol; col >= leftendcol; col--) {
                // ��Խ����
                if (col - CONTRASTOFFSET < col_min) continue;
                
                uint8_t temp1 = *(p + col);
                uint8_t temp2 = *(p + col - CONTRASTOFFSET);
                
                // ���1����㼴�ڵ㣨��ʧ�߽磩
                if (temp1 < white_min_point && col == reference_col && !leftstop) {
                    leftstop = 1;
                    for (int r = row; r >= row_min; r--) {
                        left_edge_line[r] = col_min;
                    }
                    left_search_time = 0;
                    break;
                }
                
                // ���2����ǰ��Ϊ�ڵ�
                if (temp1 < white_min_point) {
                    left_edge_line[row] = col;
                    left_search_time = 0;
                    break;
                }
                
                // ���3���Աȵ������������
                if (temp2 > white_max_point) continue;
                
                // ���4���Աȶ�������
                int16_t contrast = (temp1 - temp2) * 200 / (temp1 + temp2 + 1);
                if (contrast > reference_contrast_ratio || col == col_min) {
                    left_edge_line[row] = (col - CONTRASTOFFSET < col_min) ? col_min : (col - CONTRASTOFFSET);
                    left_search_time = 0; // �ɹ��ҵ��߽�
                    break;
                }
            }
            left_search_time--;
        } while (left_search_time > 0 && !leftstop);

        // ================== �ұ߽����� ==================
        do {
            int rightstartcol = (right_search_time == 2) ? reference_col : reference_col;
            int rightendcol = (right_search_time == 2) ? reference_col + SEARCHRANGE : col_max;
            rightendcol = (rightendcol > col_max) ? col_max : rightendcol;
            
            for (int col = rightstartcol; col <= rightendcol; col++) {
                // ��Խ����
                if (col + CONTRASTOFFSET > col_max) continue;
                
                uint8_t temp1 = *(p + col);
                uint8_t temp2 = *(p + col + CONTRASTOFFSET);
                
                // ���1����㼴�ڵ㣨��ʧ�߽磩
                if (temp1 < white_min_point && col == reference_col && !rightstop) {
                    rightstop = 1;
                    for (int r = row; r >= row_min; r--) {
                        right_edge_line[r] = col_max;
                    }
                    right_search_time = 0;
                    break;
                }
                
                // ���2����ǰ��Ϊ�ڵ�
                if (temp1 < white_min_point) {
                    right_edge_line[row] = col;
                    right_search_time = 0;
                    break;
                }
                
                // ���3���Աȵ������������
                if (temp2 > white_max_point) continue;
                
                // ���4���Աȶ�������
                int16_t contrast = (temp1 - temp2) * 200 / (temp1 + temp2 + 1);
                if (contrast > reference_contrast_ratio || col == col_max) {
                    right_edge_line[row] = (col + CONTRASTOFFSET > col_max) ? 
                                           col_max : (col + CONTRASTOFFSET);
                    right_search_time = 0; // �ɹ��ҵ��߽�
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
		
		if(row == 5){   //����Աȶ�
			prospect = SEARCH_IMAGE_H-(uint8)row;
			break;
		}
		
		if(temp1 < white_min_point){           //��ǰ���Ǻڵ�
			prospect = SEARCH_IMAGE_H-(uint8)row;
			break;
		}
		
		if(temp2 > white_max_point){           //�Աȵ��ǰ׵�
			continue;
		}
		
		
		temp3 = (temp1 - temp2)*200/(temp1 + temp2);
		
		if(temp3 >reference_contrast_ratio ){   //����Աȶ�
			prospect = SEARCH_IMAGE_H-(uint8)row;
			break;
		}
	}
//	ips200_show_int(96,224,prospect,4);

}


void Find_Edge_At_Reference_Col(const uint8_t *image) 
{
    uint8_t row;  // ��ǰ��
    uint8_t temp1, temp2;  // �Ҷ�ֵ��ʱ����
    int16_t temp3;  // �Աȶȼ�����
    
    // ��ͼ��ײ�����������ƫ�����������С��
    for (row = SEARCH_IMAGE_H - 1; row >= CONTRASTOFFSET; row--) 
    {
        // ��ȡ��ǰ����Ϸ�ƫ�Ƶ�ĻҶ�ֵ
        temp1 = *(image + row * SEARCH_IMAGE_W + reference_col);
        temp2 = *(image + (row - CONTRASTOFFSET) * SEARCH_IMAGE_W + reference_col);
        
        // ���1����ǰ���Ǻڵ㣨�����ڣ�
        if (temp1 < white_min_point) 
        {
            // �����������������������ڣ�
            continue;
        }
        
        // ���2����ǰ���ǰ׵㣨������
        if (temp1 > white_max_point) 
        {
            // ֱ�ӷ��ص�ǰ����Ϊ�߽��
            top = row;
        }
        
        // ���3������Աȶ�
        temp3 = (temp1 - temp2) * 200 / (temp1 + temp2 + 1);  // +1��ֹ����
        
        // ����Աȶ���ֵ����
        if (temp3 > reference_contrast_ratio) 
        {
            top = row;  // ���ص�ǰ����Ϊ�߽��
        }
    }
}
