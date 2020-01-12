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

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"

#include "system/inc/BtCom.h"
#include "system/inc/Btn.h"
#include "system/inc/Clock.h"
#include "system/inc/DataStorage.h"
#include "system/inc/Led.h"
#include "system/inc/WatchDog.h"
#include "sensor/inc/Pressure.h"
#include "sensor/inc/RPM.h"
#include "sensor/inc/Temp.h"
#include "Global.h"
#include "Memory.h"

// State machine states for recording to SD-Card of all sensor values.
enum class FSM : uint8_t {WAIT_SPD, WAIT_TAKEOFF, RECORDING_START, RECORDING, RECORDING_PREP_STOP, RECORDING_STOP};
using fsm_t = FSM;

// Structure to hold the information of the Built In Test (BIT).
namespace bit{
   struct components_t {
      status_t temp1;
      status_t temp2;
      status_t rpm;
      status_t press;
      status_t com;
      status_t wd;
      status_t rtc;
      status_t sto;
   };
}


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
   status_t bit_temp(SENSOR nr);
   status_t bit_pressure();
   status_t bit_rpm();
   status_t bit_watch_dog();
   status_t bit_rtc();
   status_t bit_data_storage();
   status_t bit_bt_com();

	#ifdef false
	// Is called (if enabled) and will return ONLY after a save start is reccognized.
	void save_start();
	#endif

   // For development only.
   void test_gpio();

   // Check, by the header information, the received command from the smartphone.
   void check_cmd(hdr_t hrd, Memory &data);

   // Stores interesting values of m_data onto SD-Card. Recording is only done, when a takeoff is
   // reccognized and durig flight. Both, takeoff and flight depends on certain speeds and
   // times during the speeds are present.
   status_t record(uint8_t *data, uint8_t size);

private:

   // The memory model for all sensor values.
   Memory m_data;

   // System components used.
   sys::Led m_led;			// UI, status LEDs of the uc board.
   sys::Btn m_btn;			// UI, buttons of the uc board.
   sys::BtCom m_com;		// Bluetooth communication
   sys::WatchDog m_wd;		// Watchdog.
   sys::Clock m_rtc;		// The battery backed up RTC.
   sys::DataStorage m_sto;	// SD-Card storage.

   // Local sensors used.
   sens::Temp m_temp;		// The MAX6675 sensors.
   sens::Pressure m_mb;		// The BMP280 sensor.
   sens::RPM m_rpm;			// The Round Per Minutes sensor.

   // Structure to hold the BIT information.
   bit::components_t m_bit;

   // Is a recording to SD-Card ongoing?
   bool m_recording;

   // For development only. Get the clock values of the uc.
   RCC_ClocksTypeDef m_clocks;

};

#endif /* SYSTEM_SYSCONTROL_H_ */
