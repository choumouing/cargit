#ifndef __ENCODER_H
#define __ENCODER_H


#define PIT                             (TIM6_PIT )                             // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
#define PIT_PRIORITY                    (TIM6_IRQn)                             // 对应周期中断的中断编号 在 mm32f3277gx.h 头文件中查看 IRQn_Type 枚举体

#define ENCODER_QUADDEC_L                 (TIM3_ENCODER)                //左      // 正交编码器对应使用的编码器接口 这里使用 TIM3 的编码器功能
#define ENCODER_QUADDEC_L_A               (TIM3_ENCODER_CH1_B4)                   // A 相对应的引脚
#define ENCODER_QUADDEC_L_B               (TIM3_ENCODER_CH2_B5)                   // B 相对应的引脚

#define ENCODER_QUADDEC_R                 (TIM4_ENCODER)                //右      // 正交编码器对应使用的编码器接口 这里使用 TIM4 的编码器功能
#define ENCODER_QUADDEC_R_A               (TIM4_ENCODER_CH1_B6)                   // A 相对应的引脚
#define ENCODER_QUADDEC_R_B               (TIM4_ENCODER_CH2_B7)                   // B 相对应的引脚


void Encoder_Init();
int16_t Get_Encoder_Data_Left();
int16_t Get_Encoder_Data_Right();


#endif
