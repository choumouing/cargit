#include "zf_common_headfile.h"
#include "Encoder.h"
 
int32_t encoder_data_L=0;
int32_t encoder_data_R=0;

 
 void Encoder_Init()
 {
	 encoder_quad_init(ENCODER_QUADDEC_L, ENCODER_QUADDEC_L_A, ENCODER_QUADDEC_L_B);   // 初始化编码器模块与引脚 正交解码编码器模式	 
	 encoder_quad_init(ENCODER_QUADDEC_R, ENCODER_QUADDEC_R_A, ENCODER_QUADDEC_R_B);   // 初始化编码器模块与引脚 正交解码编码器模式

 }

 int16_t Get_Encoder_Data_Left()
 {

	 encoder_data_L = encoder_get_count(ENCODER_QUADDEC_L);
	 return encoder_data_L;
 }
 
  int16_t Get_Encoder_Data_Right()
 {
	 encoder_data_R = encoder_get_count(ENCODER_QUADDEC_R);
	 return encoder_data_R;
 }
  