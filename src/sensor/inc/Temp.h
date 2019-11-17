/*
 * Temp.h
 *
 *  Created on: Sep 7, 2019
 *      Author: fox
 */

#ifndef TEMPSENS_H_
#define TEMPSENS_H_

#include <stdint.h>
#include "stm32f4xx.h"
#include "../../ReturnCodes.h"

enum SENSOR : uint8_t { T1 = 0, T2};

namespace sens{

// ************************************************************************
class Temp {
public:
	Temp();
	virtual ~Temp();
	void power_off(SENSOR nr);
	void power_on(SENSOR nr);
	uint16_t get_value(SENSOR nr);
	void setup(SENSOR nr);
	status_t bit(SENSOR nr);
private:
    void spi2_setup(void);
private:
	// element to hold port and pin information.
	typedef struct {
		GPIO_TypeDef* Port;	 // a port definition
		uint16_t Pin;		 // a pin definition
	} port_pin_t;

	// hold all ports and pins of the physical temperature sensor interface.
	// See init_gpios for activating clocks for the ports GPIOx
	struct {
		port_pin_t SCK = {GPIOB, GPIO_Pin_10};	// port, pin tuples
		port_pin_t CS = {GPIOB, GPIO_Pin_12};
		port_pin_t SO = {GPIOB, GPIO_Pin_14};
		port_pin_t VCC = {GPIOE, GPIO_Pin_14};
		port_pin_t GND = {GPIOE, GPIO_Pin_12};
		bool cable_connected = true;
	} m_t1;

    struct {
        port_pin_t SCK = {GPIOD, GPIO_Pin_12};  // port, pin tuples
        port_pin_t CS = {GPIOD, GPIO_Pin_14};
        port_pin_t SO = {GPIOC, GPIO_Pin_6};
        port_pin_t VCC = {GPIOD, GPIO_Pin_10};
        port_pin_t GND = {GPIOD, GPIO_Pin_8};
        bool cable_connected = true;
    } m_t2;
};

}
#endif /* TEMPSENS_H_ */




















