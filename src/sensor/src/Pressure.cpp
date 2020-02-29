/*
 * Pressure.cpp
 *
 *  Created on: Sep 25, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <sensor/inc/Pressure.h>
#include <tasks/Interval.h>

namespace sens {

#define SPI_ON_CNT 2
#define SPI_OFF_CNT 5
#define SPI_CS_CNT 5

//int8_t spi_reg_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
//int8_t spi_reg_read(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
//void delay_ms(uint32_t period_ms);

//typedef  int8_t (Pressure::*pSoftSPIFunc)(uint8_t, uint8_t, uint8_t *, uint16_t);
//typedef  void (Pressure::*pDelayFunc)(uint32_t);

// ************************************************************************
Pressure::Pressure() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
}

// ************************************************************************
Pressure::~Pressure() {
}

// ************************************************************************
void Pressure::setup(void) {

	GPIO_InitTypeDef GPIO_InitStructure;

	// VCC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SDI - MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// SCL
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// SDO - MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/*
	// GND
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
*/

	GPIO_ResetBits(GPIOB, GPIO_Pin_7);	// VCC
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);	// CSB
	GPIO_ResetBits(GPIOD, GPIO_Pin_7);	// SDI
	GPIO_ResetBits(GPIOD, GPIO_Pin_5);	// SCK
	GPIO_ResetBits(GPIOD, GPIO_Pin_3);	// SDO
}

// ************************************************************************
void Pressure::power_off() {
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);	// CSB
	GPIO_ResetBits(GPIOD, GPIO_Pin_7);	// SDI
	GPIO_ResetBits(GPIOD, GPIO_Pin_5);	// SCK
	GPIO_ResetBits(GPIOD, GPIO_Pin_3);	// SDO
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);	// VCC
}

// ************************************************************************
void Pressure::power_on() {

	//GPIO_ResetBits(GPIOD, GPIO_Pin_3); // GND
	GPIO_SetBits(GPIOB, GPIO_Pin_7);   // VCC
	GPIO_SetBits(GPIOB, GPIO_Pin_5);   // CSB
	GPIO_SetBits(GPIOD, GPIO_Pin_5);   // SCK

	delay_ms(500ul);
}

// ************************************************************************
status_t Pressure::bit() {

	status_t status = RC::OK;

	int8_t rslt;

	m_bmp.delay_ms = &this->delay_ms;
	m_bmp.dev_id = 0;
	m_bmp.read = &this->spi_reg_read;
	m_bmp.write = &this->spi_reg_write;
	m_bmp.intf = BMP280_SPI_INTF;

	rslt = bmp280_init(&m_bmp);

	if (rslt == BMP280_OK) {

		m_conf.spi3w_en = BMP280_SPI3_WIRE_DISABLE;
		m_conf.filter = BMP280_FILTER_OFF;
		m_conf.os_temp = BMP280_OS_1X;
		m_conf.os_pres = BMP280_OS_1X;
		m_conf.odr = BMP280_ODR_125_MS;

		rslt = bmp280_set_config(&m_conf, &m_bmp);
	}

	if (rslt == BMP280_OK) {
		rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, &m_bmp);
	}

	if (rslt != BMP280_OK) {
		status = RC::FAIL;
	}

	return status;
}

// ************************************************************************
int8_t Pressure::spi_reg_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data,
		uint16_t length) {

	GPIO_ResetBits(GPIOB, GPIO_Pin_5);        // pull cs down to start spi read.

	uint8_t data = reg_addr;

	for (uint8_t delay = 0; delay < SPI_CS_CNT; delay++)
		;

	for (uint32_t cnt = 1; cnt <= 8 * (uint32_t) length + 8; cnt++) { // write "length" bytes + 1 address byte.

		GPIO_ResetBits(GPIOD, GPIO_Pin_5);                    // clock low

		if (data & 0x80) {
			GPIO_SetBits(GPIOD, GPIO_Pin_7);                   // MOSI
		} else {
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);                 // MOSI
		}
		data <<= 1;

		for (uint8_t delay = 0; delay < SPI_OFF_CNT; delay++);

		GPIO_SetBits(GPIOD, GPIO_Pin_5);                      // clock hi

		for (uint8_t delay = 0; delay < SPI_ON_CNT; delay++);

		if (cnt % 8 == 0) {
			data = *reg_data++;
		}
	}

	GPIO_SetBits(GPIOB, GPIO_Pin_5);                    // cs hi spi read, done.

	return BMP280_OK;
}

// ************************************************************************
int8_t Pressure::spi_reg_read(uint8_t cs, uint8_t reg_addr, uint8_t* reg_data,
		uint16_t length) {

	GPIO_ResetBits(GPIOB, GPIO_Pin_5);        // pull cs down to start spi read.

	for (uint8_t delay = 0; delay < SPI_CS_CNT; delay++);

	// write address.
	for (uint32_t cnt = 0; cnt < 8; cnt++) {

		GPIO_ResetBits(GPIOD, GPIO_Pin_5);                    // clock low

		if (reg_addr & 0x80) {
			GPIO_SetBits(GPIOD, GPIO_Pin_7);                   // MOSI
		} else {
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);                 // MOSI
		}
		reg_addr <<= 1;

		for (uint8_t delay = 0; delay < SPI_OFF_CNT; delay++);

		GPIO_SetBits(GPIOD, GPIO_Pin_5);                      // clock hi

		for (uint8_t delay = 0; delay < SPI_ON_CNT; delay++);
	}

	*reg_data = 0;

	// read response.
	for (uint32_t cnt = 1; cnt <= 8 * length; cnt++) {    		// read "length" bytes

		GPIO_ResetBits(GPIOD, GPIO_Pin_5);                   	// clock low

		for (uint8_t delay = 0; delay < SPI_OFF_CNT; delay++);

		*reg_data <<= 1;
		*reg_data |= GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3); 		// read MISO pin.

		GPIO_SetBits(GPIOD, GPIO_Pin_5);                          	// clock hi

		for (uint8_t delay = 0; delay < SPI_ON_CNT; delay++);

		if (cnt % 8 == 0 && cnt != 8 * length) {
			reg_data++;
			*reg_data = 0;
		}
	}

	GPIO_SetBits(GPIOB, GPIO_Pin_5);                    // cs hi spi read, done.

	return BMP280_OK;
}

// ************************************************************************
int8_t Pressure::get_values(float& pres, float& temp) {

	int32_t t_tmp;
	uint32_t p_tmp;
	int8_t rslt;

	struct bmp280_uncomp_data ucomp_data;

	rslt = bmp280_get_uncomp_data(&ucomp_data, &m_bmp);

	bmp280_get_comp_pres_32bit(&p_tmp, ucomp_data.uncomp_press, &m_bmp);
	bmp280_get_comp_temp_32bit(&t_tmp, ucomp_data.uncomp_temp, &m_bmp);

	temp = static_cast<float>(t_tmp) * 0.01f;
	pres = static_cast<float>(p_tmp) * 0.01f;

	return rslt;
}

// ************************************************************************
void Pressure::delay_ms(uint32_t period_ms) {
	Interval::blocking_wait(period_ms);
}

}

