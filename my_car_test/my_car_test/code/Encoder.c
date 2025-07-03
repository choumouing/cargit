#include "zf_common_headfile.h"
#include "Encoder.h"
 
int16 encoder_data_L=0,encoder_data_R=0;
 
 void Encoder_Init()
 {
	 encoder_quad_init(ENCODER_QUADDEC_L, ENCODER_QUADDEC_L_A, ENCODER_QUADDEC_L_B);   // 初始化编码器模块与引脚 正交解码编码器模式
	 encoder_quad_init(ENCODER_QUADDEC_R, ENCODER_QUADDEC_R_A, ENCODER_QUADDEC_R_B);   // 初始化编码器模块与引脚 正交解码编码器模式
	 pit_ms_init(PIT, 100);                                                      // 初始化 PIT 为周期中断 100ms 周期
   interrupt_set_priority(PIT_PRIORITY, 0);                                    // 设置 PIT 对周期中断的中断优先级为 0
 }

 int16 Get_Encoder_Data_Left()
 {
	 return encoder_data_L;
 }
 
  int16 Get_Encoder_Data_Right()
 {
	 return encoder_data_R;
 }
 