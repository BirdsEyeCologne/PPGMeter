/*
 * Btns.h
 *
 *  Created on: Sep 17, 2019
 *      Author: fox
 */

#ifndef BTN_H_
#define BTN_H_

#include <stdint.h>
#include "stm32f4xx.h"

enum BTN : uint8_t { K_UP = 0, K0, K1};

namespace sys{


// ************************************************************************
class Btn {
public:
	Btn();
	virtual ~Btn();
	void setup(BTN id);
};

}
#endif
