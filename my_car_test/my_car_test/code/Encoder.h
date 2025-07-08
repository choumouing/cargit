#ifndef __ENCODER_H
#define __ENCODER_H


#define PIT                             (TIM6_PIT )                             // ʹ�õ������жϱ�� ����޸� ��Ҫͬ����Ӧ�޸������жϱ���� isr.c �еĵ���
#define PIT_PRIORITY                    (TIM6_IRQn)                             // ��Ӧ�����жϵ��жϱ�� �� mm32f3277gx.h ͷ�ļ��в鿴 IRQn_Type ö����

#define ENCODER_QUADDEC_L                 (TIM3_ENCODER)                //��      // ������������Ӧʹ�õı������ӿ� ����ʹ�� TIM3 �ı���������
#define ENCODER_QUADDEC_L_A               (TIM3_ENCODER_CH1_B4)                   // A ���Ӧ������
#define ENCODER_QUADDEC_L_B               (TIM3_ENCODER_CH2_B5)                   // B ���Ӧ������

#define ENCODER_QUADDEC_R                 (TIM4_ENCODER)                //��      // ������������Ӧʹ�õı������ӿ� ����ʹ�� TIM4 �ı���������
#define ENCODER_QUADDEC_R_A               (TIM4_ENCODER_CH1_B6)                   // A ���Ӧ������
#define ENCODER_QUADDEC_R_B               (TIM4_ENCODER_CH2_B7)                   // B ���Ӧ������


void Encoder_Init();
int16_t Get_Encoder_Data_Left();
int16_t Get_Encoder_Data_Right();


#endif
