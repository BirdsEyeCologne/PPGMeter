/*
 * WatchDog.cpp
 *
 *  Created on: Sep 17, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <system/inc/WatchDog.h>

volatile uint32_t uwCaptureNumber = 0;
volatile uint32_t uwPeriodValue = 0;

namespace sys{

// ************************************************************************
WatchDog::WatchDog() {}

// ************************************************************************
WatchDog::~WatchDog() {}

// ************************************************************************
void WatchDog::setup(const uint16_t timeout_sec) {

	uint32_t lsi_freq;

	/* Get the LSI frequency:  TIM5 is used to measure the LSI frequency */
	lsi_freq = get_lsi_frequency();

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	IWDG_SetPrescaler(IWDG_Prescaler_256);

	IWDG_SetReload(timeout_sec * lsi_freq / 256);

	IWDG_ReloadCounter();

	IWDG_Enable();
}

// ************************************************************************
wd_state_t WatchDog::check_for_rst(void) {

   wd_state_t wd_reset = WD_STATE::OK;

	// Reseted by WD?
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) {
		RCC_ClearFlag();
		wd_reset = WD_STATE::DID_RST;
	}

	return wd_reset;
}

// ************************************************************************
void WatchDog::refresh(void) {
#ifdef IWDG_ENABLE
   IWDG_ReloadCounter();
#endif
}

// ************************************************************************
uint32_t WatchDog::get_lsi_frequency(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	RCC_ClocksTypeDef RCC_ClockFreq;

	/* Enable the LSI oscillator ************************************************/
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
	   ;;
	}

	/* TIM5 configuration *******************************************************/
	/* Enable TIM5 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
	TIM_RemapConfig(TIM5, TIM5_LSI);

	/* Configure TIM5 presclaer */
	TIM_PrescalerConfig(TIM5, 0, TIM_PSCReloadMode_Immediate);

	/* TIM5 configuration: Input Capture mode ---------------------
	 The LSI oscillator is connected to TIM5 CH4
	 The Rising edge is used as active edge,
	 The TIM5 CCR4 is used to compute the frequency value
	 ------------------------------------------------------------ */
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	/* Enable TIM5 Interrupt channel */
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable TIM5 counter */
	TIM_Cmd(TIM5, ENABLE);

	/* Reset the flags */
	TIM5->SR = 0;

	/* Enable the CC4 Interrupt Request */
	TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);

	/* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in stm32f4xx_it.c file) ******************************************************/
	while (uwCaptureNumber != 2) {
	   ;;
	}
	/* Deinitialize the TIM5 peripheral registers to their default reset values */
	TIM_DeInit(TIM5);

	/* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
	/* Get SYSCLK, HCLK and PCLKx frequency */
	RCC_GetClocksFreq(&RCC_ClockFreq);

	/* Get PCLK1 prescaler */
	if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0) {
		/* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
		return ((RCC_ClockFreq.PCLK1_Frequency / uwPeriodValue) * 8);
	} else { /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
		return (((2 * RCC_ClockFreq.PCLK1_Frequency) / uwPeriodValue) * 8);
	}
}

}
