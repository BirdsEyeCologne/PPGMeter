/*
 * BtCom.cpp
 *
 *  Created on: Sep 7, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <system/inc/BtCom.h>

volatile uint8_t uart1_rx_buff[UART1_BUFFER_SIZE];
volatile uint8_t uart1_rx_cnt;

extern volatile uint32_t ms_cnt;

namespace sys {

// ************************************************************************
BtCom::BtCom() :
		m_at_reset("AT+RESET\r\n") {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	uart1_rx_cnt = 0;
}

// ************************************************************************
BtCom::~BtCom() {
	GPIO_DeInit(GPIOA);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
}

// ************************************************************************
void BtCom::setup(BAUD_RATE br) {

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// Tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Rx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// EN for enabling AT command mode.
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	// VCC pin for BT module.
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = (uint32_t) br;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART1, ENABLE);
}

// ************************************************************************
void BtCom::tx_header(resp_hdr_t header) {
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
	USART1->DR = (uint8_t) header; // Frame start
}

// ************************************************************************
hdr_t BtCom::rx_data(uint8_t * data) {

	hdr_t cmd = HDR::NO_CMD;

	if (uart1_rx_cnt == BT_PAYLOAD) {
		cmd = (hdr_t) uart1_rx_buff[0];
		std::memcpy(data, (void*) &uart1_rx_buff[1], BT_PAYLOAD - 1);
	}

	uart1_rx_cnt = 0;

	return cmd;
}

// ************************************************************************
void BtCom::power_on() {

	GPIO_SetBits(GPIOB, GPIO_Pin_9);     // Set pin high, to power BT module.

	// Wait for settle of module.
	ms_cnt = 500;
	while (ms_cnt != 0)
		;
}

// ************************************************************************
void BtCom::power_off() {
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);   // Pull pin low, to unpower BT module.
}

// ************************************************************************
status_t BtCom::bit(void) {

	status_t status = RC::NC;

	uart1_rx_cnt = 0;
	tx_data(m_at_reset, sizeof(m_at_reset) - 1); // Bt module reset from at mode.

	ms_cnt = 800;
	while (ms_cnt != 0 && uart1_rx_cnt != 4);  // Wait for 800 ms timeout, or receiption of 'OK\r\n' from BT module.

	if (uart1_rx_cnt == 4 && uart1_rx_buff[0] == 'O' && uart1_rx_buff[1] == 'K') {
		status = RC::OK;
	} else {
		status = RC::NC;
	}
	uart1_rx_cnt = 0;

	return status;
}

// ************************************************************************
void BtCom::tx_data(uint8_t * data, uint8_t size) {

	for (uint8_t cnt = 0; cnt < size; cnt++) {
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART1->DR = data[cnt];
	}

}

// ************************************************************************
void BtCom::set_at_mode(bool state) {

	if (state == true) {
		GPIO_SetBits(GPIOE, GPIO_Pin_1); // Set pin high, to enter AT mode. Needs to be high at power on of the module.
	} else {
		GPIO_ResetBits(GPIOE, GPIO_Pin_1);    // Pull pin low, to leave AT mode.
	}
}

}

