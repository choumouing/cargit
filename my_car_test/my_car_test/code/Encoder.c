#include "zf_common_headfile.h"
#include "Encoder.h"
 
int16 encoder_data_L=0,encoder_data_R=0;
 
 void Encoder_Init()
 {
	 encoder_quad_init(ENCODER_QUADDEC_L, ENCODER_QUADDEC_L_A, ENCODER_QUADDEC_L_B);   // ��ʼ��������ģ�������� �������������ģʽ
	 encoder_quad_init(ENCODER_QUADDEC_R, ENCODER_QUADDEC_R_A, ENCODER_QUADDEC_R_B);   // ��ʼ��������ģ�������� �������������ģʽ
	 pit_ms_init(PIT, 100);                                                      // ��ʼ�� PIT Ϊ�����ж� 100ms ����
   interrupt_set_priority(PIT_PRIORITY, 0);                                    // ���� PIT �������жϵ��ж����ȼ�Ϊ 0
 }

 int16 Get_Encoder_Data_Left()
 {
	 return encoder_data_L;
 }
 
  int16 Get_Encoder_Data_Right()
 {
	 return encoder_data_R;
 }
 