/*
 * RTC.h
 *
 *  Created on: Sep 7, 2019
 *      Author: fox
 */

#ifndef CLOCK_H_
#define CLOCK_H_
#include <stm32f4xx.h>
#include "../../Global.h"

namespace sys{


// ************************************************************************
class Clock {
public:
	Clock();
	virtual ~Clock();
	void setup_once();

	// Write new time and date to the RTC
	void set_date(RTC_DateTypeDef today);
	void set_time(RTC_TimeTypeDef now);
	RTC_DateTypeDef get_date(void);
	RTC_TimeTypeDef get_time(void);

	// Handle motor houres.
	uint32_t get_measure(void);
	void reset_measure(void);
	void measure_start(void);
	void measure_stop(void);

private:
	 RTC_TimeTypeDef m_now;
	 RTC_DateTypeDef m_today;
	 uint32_t m_engine_hours; 		// Engine hours in seconds.
	 bool m_engine_hours_counting;	// Counting the engine hours?
};

}
#endif
