/*
 * Gpio.cpp
 *
 *  Created on: Sep 17, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <system/inc/Btn.h>

namespace sys{


// ************************************************************************
Btn::Btn() {}

// ************************************************************************
Btn::~Btn() {}

// ************************************************************************
void Btn::setup(BTN id) {

   GPIO_InitTypeDef GPIO_InitStruct;
   EXTI_InitTypeDef EXTI_InitStruct;
   NVIC_InitTypeDef NVIC_InitStruct;

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

   if (id == BTN::K1) {
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
      GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
      GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
      GPIO_Init(GPIOE, &GPIO_InitStruct);

      SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);

      EXTI_InitStruct.EXTI_Line = EXTI_Line3;
      EXTI_InitStruct.EXTI_LineCmd = ENABLE;
      EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
      EXTI_Init(&EXTI_InitStruct);

      NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
      NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
      NVIC_InitStruct.NVIC_IRQChannelSubPriority = 10;
      NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStruct);
   }

   if (id == BTN::K0) {
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
      GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
      GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
      GPIO_Init(GPIOE, &GPIO_InitStruct);

      SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);

      EXTI_InitStruct.EXTI_Line = EXTI_Line4;
      EXTI_InitStruct.EXTI_LineCmd = ENABLE;
      EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
      EXTI_Init(&EXTI_InitStruct);

      NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
      NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
      NVIC_InitStruct.NVIC_IRQChannelSubPriority = 10;
      NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStruct);
   }
}

}
