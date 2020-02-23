/*
 * =====================================================================================
 *
 *       Filename:  pwmOutput.c
 *
 *    Description:  PWM输出控制模块，使用TIM2定时器
 *
 *        Version:  
 *        Created:  
 *       Revision:  
 *       Compiler:  
 *
 *         Author:  zhangyuxiang
 *   Organization:  
 *
 * =====================================================================================
 */

#include "stm32f10x.h"
#include "common.h"
#include "pwmOutput.h"

typedef void (*TIM_OCxInit_t)(TIM_TypeDef* , TIM_OCInitTypeDef*);
const TIM_OCxInit_t TIM_OCxInit[4] = {TIM_OC1Init, TIM_OC2Init, TIM_OC3Init, TIM_OC4Init};
const uint16_t ITx[4] = {TIM_IT_CC1, TIM_IT_CC2, TIM_IT_CC3, TIM_IT_CC4};

static void TIM2_Output_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_GPIOClockCmd(GPIOA, ENABLE); 

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIMx_Config(TIM_TypeDef* TIMx, uint16_t period, uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

	/* Time base configuration */		 
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;	        //设置预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                //设置时钟分频系数
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式

	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

	TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);

	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);

	TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);

	TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIMx, ENABLE);

	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
}

void TIMx_OCx_Config(TIM_TypeDef* TIMx, int OCx, uint16_t pulse, FunctionalState output)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;       //配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState = output;   
    TIM_OCInitStructure.TIM_Pulse = pulse;      //设置跳变值，当计数器计数到这个值时，电平发生跳变
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  //当定时器计数值小于CCR1_Val时为高电平

    TIM_OCxInit[OCx - 1](TIMx, &TIM_OCInitStructure);

    // TIM_ITConfig(TIMx, ITx[OCx - 1], ENABLE);
}

void TIM2_SetInterrupt(FunctionalState state)
{  
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //指定中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   // 指定响应优先级别
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;
    NVIC_Init(&NVIC_InitStructure);
}

void PWM_TIM2_Interrupt(void)
{
    int i;
    for(i = 0; i < 4; i++){
        ITStatus s = TIM_GetITStatus(TIM2, ITx[i]);
        if(s == SET) {
            TIM_ClearITPendingBit(TIM2, ITx[i]);
            // OCx_Interrupt_Handler(i + 1);
        }
    }
    if(SET == TIM_GetITStatus(TIM2, TIM_IT_Update)) {
    	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    	GP2Y1010_IntHandler();
    }
}
