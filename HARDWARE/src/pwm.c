#include "pwm.h"

//uint32_t CCR1 = 1800;
//uint32_t CCR2 = 1800;
uint32_t CCR3 = 1800;

void PWM_Init(uint32_t arr,uint16_t psc,uint32_t deadtime)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
		
	TIM_DeInit(TIM1);
	TIM_InternalClockConfig(TIM1);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;// GPIO_Pin_8 | GPIO_Pin_9 |
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;//GPIO_Pin_13 | GPIO_Pin_14 |
	GPIO_Init(GPIOB,&GPIO_InitStructure);
 
	TIM_TimeBaseStructure.TIM_Period = arr-1;
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
//	TIM_OCInitStructure.TIM_Pulse = CCR1;
//	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
//	
//	TIM_OCInitStructure.TIM_Pulse = CCR2;
//	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_Pulse = CCR3;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSRState_Disable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStructure.TIM_DeadTime = deadtime;
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
//	
//	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  
//	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
		
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE); 
	TIM_Cmd(TIM1, ENABLE);
		
}