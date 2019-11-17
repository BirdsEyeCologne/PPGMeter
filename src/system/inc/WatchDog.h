/*
 * WatchDog.h
 *
 *  Created on: Sep 17, 2019
 *      Author: fox
 */

#ifndef SYSTEM_WATCHDOG_H_
#define SYSTEM_WATCHDOG_H_

#include <cstdint>
#include "stm32f4xx.h"

enum class WD_STATE : uint8_t {OK, DID_RST};
typedef WD_STATE wd_state_t;

namespace sys{

// ************************************************************************
class WatchDog {
public:
	WatchDog();
	virtual ~WatchDog();
	void setup(const uint16_t timeout_sec);
	wd_state_t check_for_rst(void);
	void refresh(void);
private:
	uint32_t get_lsi_frequency(void);
};

}
#endif /* SYSTEM_WATCHDOG_H_ */
