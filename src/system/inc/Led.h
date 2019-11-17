/*
 * Gpio.h
 *
 *  Created on: Sep 7, 2019
 *      Author: fox
 */

#ifndef LED_H_
#define LED_H_

#include <stdint.h>
#include "stm32f4xx.h"

enum class LED{D2, D3};

namespace sys{
// ************************************************************************
class Led {
public:
	Led();
	virtual ~Led();

	//@brief Indicator LED stuff.
	void setup(LED led);
	void toggle(LED led);
	void off(LED led);
	void on(LED led);
};
}
#endif /* GPIO_H_ */
