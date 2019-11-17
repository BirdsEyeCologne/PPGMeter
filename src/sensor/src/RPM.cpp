/*
 * RPM.cpp
 *
 *  Created on: Sep 7, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <cstdint>
#include <sensor/inc/RPM.h>

volatile uint32_t rpm_cnt;

namespace sens {

// ************************************************************************
RPM::RPM() {}

// ************************************************************************
RPM::~RPM() {}

// ************************************************************************
void RPM::setup() {

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

   GPIO_InitTypeDef GPIO_InitStructure;

   // RPM/WK_UP pin
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   // Emergency Halt pin
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   /*
   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

   EXTI_InitStruct.EXTI_Line = EXTI_Line0;
   EXTI_InitStruct.EXTI_LineCmd = ENABLE;
   EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
   EXTI_Init(&EXTI_InitStruct);

   NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
   NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
   NVIC_InitStruct.NVIC_IRQChannelSubPriority = 10;
   NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStruct);
*/


   // Configures 0.1 ms timer for delta t measurements.
   // **********************************************************
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

   TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
   TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // Divides system clock by 1,2 or 4.
   TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBase_InitStructure.TIM_Period = 100 - 1;	// 10 kHz timer clock -> 0.1 ms intervall.
   TIM_TimeBase_InitStructure.TIM_Prescaler = 84;	// 84 MHz clock -> 1 MHz timer clock
   TIM_TimeBaseInit(TIM2, &TIM_TimeBase_InitStructure);
   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

   NVIC_InitTypeDef NVIC_TIM_InitStructure;
   NVIC_TIM_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  // <- NVIC Mandatory for timer to work properly with interrupt!!
   NVIC_TIM_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_TIM_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
   NVIC_TIM_InitStructure.NVIC_IRQChannelSubPriority = 10;
   NVIC_Init(&NVIC_TIM_InitStructure);
   TIM_Cmd(TIM2, ENABLE);

   rpm_cnt = 0;
}

// ************************************************************************
status_t RPM::bit() {
   return RC::OK;
}

// ************************************************************************
uint16_t RPM::get_value() {
   uint16_t rpm = rpm_cnt;
   rpm_cnt = 0;
   return rpm;
}




}


