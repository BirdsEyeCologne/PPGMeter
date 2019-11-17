/*
 * RPM.h
 *
 *  Created on: Sep 7, 2019
 *      Author: fox
 */

#ifndef RPM_H_
#define RPM_H_

#include "stm32f4xx.h"
#include "../../ReturnCodes.h"

namespace sens{

// ************************************************************************
class RPM {
public:
	RPM();
	virtual ~RPM();
	void setup();
	status_t bit();
	uint16_t get_value();
};

}

#endif /* RPMSENS_H_ */
