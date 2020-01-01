/*
 * Gpio.cpp
 *
 *  Created on: Sep 7, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <system/inc/Led.h>
namespace sys{
// ************************************************************************
Led::Led() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
}

// ************************************************************************
Led::~Led() {
	GPIO_DeInit(GPIOA);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE);
}

// ************************************************************************
void Led::setup(LED led) {

	GPIO_InitTypeDef GPIO_InitStructure;

	// Save start/ record indicator.
	if (led == LED::D3) {
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		//GPIO_SetBits(GPIOA, GPIO_Pin_8);
	}

	// Alive indicator.
	if (led == LED::D2) {
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		//GPIO_SetBits(GPIOD, GPIO_Pin_13);
	}
}

// ************************************************************************
void Led::toggle(LED led) {
	if (led == LED::D3) {
		GPIO_ToggleBits(GPIOA, GPIO_Pin_8);
	}
	if (led == LED::D2) {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
	}
}

// ************************************************************************
void Led::off(LED led) {
	if (led == LED::D3) {
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	}
	if (led == LED::D2) {
		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
	}
}

// ************************************************************************
void Led::on(LED led) {
	if (led == LED::D3) {
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
	}
	if (led == LED::D2) {
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
	}
}
}
