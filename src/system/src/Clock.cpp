/*
 * RTC.cpp
 *
 *  Created on: Sep 7, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <system/inc/Clock.h>

#define RTC_CLOCK_SOURCE_LSE

namespace sys {

// ************************************************************************
Clock::Clock() {
	m_now.RTC_Seconds = 0;
	m_now.RTC_Minutes = 0;
	m_now.RTC_Hours = 0;
	PWR_BackupAccessCmd(ENABLE);
	m_measure_time = RTC_ReadBackupRegister(RTC_BKP_DR1);
	m_measuring = false;
}

// ************************************************************************
Clock::~Clock() {
}

// ************************************************************************
void Clock::setup() {

	RTC_InitTypeDef RTC_InitStructure;

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* Enable the LSE OSC */
	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* ck_spre(1Hz) = RTCCLK(LSE) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
	/* Configure the RTC data register and RTC prescaler */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

//	/* Set the alarm 05h:20min:30s */
//	RTC_AlarmTypeDef RTC_AlarmStructure;
//	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
//	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = 0x05;
//	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x20;
//	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x30;
//	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
//	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
//	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
//
//	/* Configure the RTC Alarm A register */
//	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
//
//	/* Enable RTC Alarm A Interrupt */
//	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
//
//	/* Enable the alarm */
//	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
//
//	RTC_ClearFlag (RTC_FLAG_ALRAF);

	RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_SETUP_WORD);
	RTC_WriteBackupRegister(RTC_BKP_DR1, 0);
}

// ************************************************************************
void Clock::set_date(RTC_DateTypeDef today) {
	RTC_SetDate(RTC_Format_BIN, &today);
	RTC_GetDate(RTC_Format_BIN, &m_today);
}

// ************************************************************************
void Clock::set_time(RTC_TimeTypeDef now) {
	RTC_SetTime(RTC_Format_BIN, &now);
	RTC_GetTime(RTC_Format_BIN, &m_now);
}

// ************************************************************************
RTC_DateTypeDef Clock::get_date(void) {
	RTC_DateTypeDef today;
	RTC_GetDate(RTC_Format_BIN, &today);
	return today;
}

// ************************************************************************
RTC_TimeTypeDef Clock::get_time(void) {
	RTC_TimeTypeDef now;
	RTC_GetTime(RTC_Format_BIN, &now);
	return now;
}

// ************************************************************************
uint32_t Clock::get_measure(void) {

	uint32_t since_measure_start = 0;

	if(m_measuring == true){

		RTC_TimeTypeDef now;

		RTC_GetTime(RTC_Format_BIN, &now);

		since_measure_start = 	(now.RTC_Seconds - m_now.RTC_Seconds) +
								(now.RTC_Minutes - m_now.RTC_Minutes) * 60 +
								(now.RTC_Hours - m_now.RTC_Hours) * 3600;

		RTC_WriteBackupRegister(RTC_BKP_DR1, m_measure_time + since_measure_start);
	}

	return m_measure_time + since_measure_start;
}

// ************************************************************************
void Clock::reset_measure(void) {
	m_measure_time = 0;
	RTC_WriteBackupRegister(RTC_BKP_DR1, m_measure_time);
}

// ************************************************************************
void Clock::measure_start(void) {

	if(m_measuring == false){
		m_measure_time = RTC_ReadBackupRegister(RTC_BKP_DR1);
		RTC_GetTime(RTC_Format_BIN, &m_now);
		RTC_GetDate(RTC_Format_BIN, &m_today);
		m_measuring = true;
	}
}

// ************************************************************************
void Clock::measure_stop(void) {

	if(m_measuring == true){

		RTC_TimeTypeDef now;

		RTC_GetTime(RTC_Format_BIN, &now);

		m_measure_time +=
				(now.RTC_Seconds - m_now.RTC_Seconds) +
				(now.RTC_Minutes - m_now.RTC_Minutes) * 60 +
				(now.RTC_Hours - m_now.RTC_Hours) * 3600 ;

		RTC_WriteBackupRegister(RTC_BKP_DR1, m_measure_time);

		m_measuring = false;
	}
}



}



















