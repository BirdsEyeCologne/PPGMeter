/*
 * Control.h
 *
 *  Created on: Sep 7, 2019
 *      Author: fox
 */

#ifndef SYSTEM_SYSCONTROL_H_
#define SYSTEM_SYSCONTROL_H_

#include <cstdint>
#include <cstring>
#include <vector>

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"

#include "system/inc/Btn.h"
#include "system/inc/Clock.h"
#include "system/inc/Led.h"
#include "system/inc/WatchDog.h"
#include "tasks/ReccuringTask.h"
#include "Global.h"
#include "Memory.h"
#include "ReturnCodes.h"

// ****************************************************************
class SysControl {

public:
   explicit SysControl();
   virtual ~SysControl();
   void run(void);

private:

   // Setup of SysControl.
   void setup();

   // Startup Built In Test (BIT) of sensor and system components.
   void startup_bit();
   status_t bit_watch_dog();

	#ifdef false
	// Is called (if enabled) and will return ONLY after a save start is reccognized.
	void save_start();
	#endif

   // For development only.
   void test_gpio();

private:

   // The memory model for all sensor values.
   Memory m_memory;

   // System components used.
   sys::Led m_led;			// UI, status LEDs of the uc board.
   sys::Btn m_btn;			// UI, buttons of the uc board.
   sys::WatchDog m_wd;		// Watchdog.
   sys::Clock m_rtc;		// The battery backed up RTC.

   // For development only. Get the clock values of the uc.
   RCC_ClocksTypeDef m_clocks;

   // List of reccuring tasks
   std::vector<std::unique_ptr<task::ReccuringTask>> m_tasks;

};

#endif /* SYSTEM_SYSCONTROL_H_ */
