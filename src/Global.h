/*
 * Externals.h
 *
 *  Created on: Sep 19, 2019
 *      Author: fox
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

// Comment to disable.
#define AUTO_SHUTDOWN 60		// Automatic shutdown of the PPGMeter, in seconds, will happen is no recording is going on.

// Comment to disable.
#define SAVE_START               // Save start monitors the RPM at startup. If the RPM are to hight, the motor is turned off.

// Save Start / Emergency Halt parameter. Note: there are 2 ignitions per revolution on Polini Thor 190.
#define RPM_EM_HALT 4000UL              // Rpm at which emergency halt is issued.
#define RPM_EM_HALT_US (600000UL/(2*RPM_EM_HALT)) // Delta us*100 for emergency halt rpm.
#define RPM_EM_HALT_CNT (2*3)   // Halt after 3 full revolutions with emergency halt rpm.
#define RPM_EM_HALT_WAIT (2*410)// Wait for 410 revolutions for emergency halt. At 2500 rpm this results in ~ 10 sec wait.

#define BIT_RETRY 5				// How many times should a bit be retried, before disable the component.

//#define IWDG_ENABLE           // Comment for disable intependent watch dog.
#define IWDG_TIMEOUT_SEC 3      // Timeouts after x seconds. note: max timeout in seconds is limited.

#define DEBUG_RPM_ENABLE        // Comment for disable dummy rpm signal on pin PD15.

#define RPM_SCALE (60/2)        // 2 ignitions per revolution and 60 times per second = 60/2 = 30.

#define UART1_BUFFER_SIZE 64    // BT Com receive buffer.

#define RTC_SETUP_WORD (0x32F2)	// This value is written to RTC register 0 to indicate the initial setup of the RTC had been done.

#define SPD_TAKEOFF 15 			// Minimum speed (km/h) to recognize takeoff
#define SPD_TAKEOFF_SECS 5		// Minimum time of takeoff speed to trigger recording of flight.
#define SPD_FLIGHT 5 			// Minimum speed (km/h) to recognize flight
#define SPD_FLIGHT_SECS 10		// Minimum time of flight speed to trigger stop recording of flight.

#define PATH_MAX 100			// Maximum size of path variable for SD-Card storage
#define FILES_PER_DAY_MAX 240	// Maximum of lights recordable on one day.

#define SD_ERROR_DISABLE 3		// After this many SD-Card errors, the SD-Card gets disabled.

#endif /* GLOBAL_H_ */
