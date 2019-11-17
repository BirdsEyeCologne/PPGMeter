/*
 * Control.cpp
 *
 *  Created on: Sep 7, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <SysControl.h>

extern volatile uint32_t rpm_cnt;

volatile uint32_t ms_cnt;
volatile uint32_t ms_clock;
volatile uint32_t startup_wait;
volatile uint8_t time_sync;

// ****************************************************************
SysControl::SysControl() {

	setup();

	// Only returns if a save start was done (if enabled).
	#ifdef SAVE_START
	save_start();
	#endif

	// Startup Built In Test (BIT) of coponents.
	system_bit();
	sensor_bit();
}

// ****************************************************************
SysControl::~SysControl() {}

// ****************************************************************
void SysControl::setup() {

	// Init stuff.
	m_recording = false;
	time_sync = 0;

	// Get clock values as the uc expects them.
	RCC_GetClocksFreq(&m_clocks);

	// Setup for UI, 2 status led and 2 user btn.
	m_led.setup(LED::D2);
	m_led.setup(LED::D3);
	m_btn.setup(BTN::K0);
	m_btn.setup(BTN::K1);

	// Configures 1 ms timer for ms clock signal ms_clock.
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	NVIC_InitTypeDef NVIC_TIM_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // Divides system clock by 1,2 or 4.
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Period = 500 - 1;
	TIM_TimeBase_InitStructure.TIM_Prescaler = 168;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBase_InitStructure);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	NVIC_TIM_InitStructure.NVIC_IRQChannel = TIM4_IRQn; // <- NVIC Mandatory for timer to work properly with interrupt!!
	NVIC_TIM_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_TIM_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
	NVIC_TIM_InitStructure.NVIC_IRQChannelSubPriority = 10;
	NVIC_Init(&NVIC_TIM_InitStructure);

	TIM_Cmd(TIM4, ENABLE);

	// Enable WKUP pin. Interrupt on this pin will wakeup uc.
	// This pin is also the source for the RPM counter.
	PWR_WakeUpPinCmd(ENABLE);

	#ifdef DEBUG_RPM_ENABLE
	// Dummy RPM on pin D15 for testing...
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	#endif

	// Setup of rpm interrupts etc. needed for save_start / em halt!
	m_rpm.setup();

}

// ****************************************************************
void SysControl::run() {

	// Infinite loop. Only way out is some sort of fault (segfault etc.), or
	// automatic shutdown / standby (if enabled).
	while (true) {

		ms_cnt = 1000;					// There is a wait loop at the end, which waits for 1000 ms after all the work is done.

		m_led.toggle(LED::D2);			// Flash LED as alive indicator :)

		if(m_recording == true){		// Indicate recording flight data.
			m_led.off(LED::D3);
		}else{
			m_led.on(LED::D3);
		}

		// Temp 1 *****************************************
		if (m_sen.temp1 == RC::OK) {
			m_data.t1 = m_temp.get_value(SENSOR::T1);
		}

		// Temp 2 *****************************************
		if (m_sen.temp2 == RC::OK) {
			m_data.t2 = m_temp.get_value(SENSOR::T2);
		}

		// Pressure (p) and Temp 3 ************************
		if (m_sen.press == RC::OK) {
			if (m_mb.get_values(m_data.p, m_data.t3) != BMP280_OK) {
				m_data.p = 0;
				m_data.t3 = 0;
			}
			m_data.t3 /= 100;	// "Round" to full degrees.
		}

		// RPM ********************************************
		if (m_sen.rpm == RC::OK) {

			static bool spinning = false;

			m_data.rpm = m_rpm.get_value();
			m_data.rpm *= RPM_SCALE;		// Scaling for Round Per Minute (RPM).
			m_data.rpm /= 100;             	// "Round" rpm value to full hundreds of RPMs.
			m_data.rpm *= 100;

			// Trigger total time measuring of the motor spinning (motor hour meter).
			if (m_data.rpm != 0 && spinning == false) {
				m_rtc.measure_start();
				spinning = true;
			}
			else if (m_data.rpm == 0 && spinning == true) {
				m_rtc.measure_stop();
				spinning = false;
				ms_clock = 0;
			}
			// Automatic shutdown of the uc if there is no RPM for AUTO_SHUTDOWN seconds.
			#ifdef AUTO_SHUTDOWN
			// Shutdown if motor is not spinning and no recording is going on.
			else if (m_data.rpm == 0 && spinning == false && ms_clock >= 1000 * AUTO_SHUTDOWN && m_recording == false){
				PWR_EnterSTANDBYMode();
			}
			#endif
		}

		// Get motor hour counter (h) *********************
		if (m_sys.rtc == RC::OK) {
			m_data.h = m_rtc.get_measure();
		}

		// BT transceive **********************************
		if (m_sys.com == RC::OK) {

			// Transmit sensor values to smartphone.
			m_com.tx_header(RESP_HDR::REPORT_DATA);
			m_com.tx_data(m_data.mem.data(), 16);

			// Receive sensor values from smartphone.
			hdr_t hdr = m_com.rx_data(m_data.mem.data() + 16);

			// Check received command by header information.
			check_cmd(hdr, m_data);

/*
			// Simulation of takeoff, flight and landing :)
			static uint8_t cnt = 0;
			cnt++;
			if(cnt >= 20 && cnt <= 60 && m_data.bt_valid == true){
				m_data.spd = 40;
			}
*/
		}

		// Sync time with smartphone **********************
		if (time_sync == 1) { // User has pressed BTN::K0.
			if (m_data.bt_valid == true) {

				RTC_TimeTypeDef now = {0,0,0,0};
				RTC_DateTypeDef today = {0,0,0,0};

				// Use the data stored on m_data, which is sent
				// via bluetooth from the smartphone.
				now.RTC_Minutes = m_data.min;
				now.RTC_Hours = m_data.hour;
				today.RTC_Date = m_data.day;
				today.RTC_Month = m_data.month;
				today.RTC_Year = m_data.year;

				m_rtc.set_time(now);
				m_rtc.set_date(today);
			}
			time_sync = 0;
		}

		// SD-Card storage ********************************
		if (m_sys.sto == RC::OK) {

			// Get the values of the local RTC, not the values
			// stored on m_data, which is from the smartphone.
			// Most likely both times are eaqual. Regular synch of
			// uc clock is recomended.
			RTC_TimeTypeDef now = m_rtc.get_time();

			m_data.sec = now.RTC_Seconds;	// Dirty hack, see Memory.h (sec and day)
			m_data.min = now.RTC_Minutes;
			m_data.hour = now.RTC_Hours;

			// Record sensor data to the SD-Card memory.
			// Recording is based on speed and time (takeoff/flight/landed).
			status_t status = record(m_data.mem.data(), 31);

			// If to many recording errors have happened, disable storage.
			if(status == RC::ERROR){
				static uint8_t error = 0;
				if(error++ >= SD_ERROR_DISABLE){
					m_sys.sto = RC::FAIL;
				}
			}

			// Invalidate the data. Only freshly received
			// data should be stored onto SD-Card.
			m_data.bt_valid = false;
		}

		m_wd.refresh();

		while (ms_cnt != 0);
	}
}

// ****************************************************************
status_t SysControl::record(uint8_t * data, uint8_t size) {

	static fsm_t state = FSM::WAIT_SPD;
	status_t status = RC::OK;

	// Check if valid data is received from smartphone. If valid data is
	// present, analyze to decide if it should be stored on SD-Card.
	if (m_data.bt_valid == true) {

		// ****************************************************
		if (state == FSM::WAIT_SPD) {
			if (m_data.spd >= SPD_TAKEOFF) {
				ms_clock = 0;
				state = FSM::WAIT_TAKEOFF;
			}
		}

		// ****************************************************
		if (state == FSM::WAIT_TAKEOFF) {
			if (m_data.spd >= SPD_TAKEOFF && ms_clock >= 1000 * SPD_TAKEOFF_SECS) {
				state = FSM::RECORDING_START;
			} else if (m_data.spd < SPD_TAKEOFF) {
				state = FSM::WAIT_SPD;
			}
		}

		// ****************************************************
		if (state == FSM::RECORDING_START) {
			// For folder structure: storage_root/year/month/day/#.bin (where # is the fligt number/counter).
			RTC_DateTypeDef today = m_rtc.get_date();
			status = m_sto.open(today);
			m_recording = true;
			state = FSM::RECORDING;
		}

		// ****************************************************
		if (state == FSM::RECORDING) {
			if (m_data.spd < SPD_FLIGHT) {
				state = FSM::RECORDING_PREP_STOP;
				ms_clock = 0;
			}
			status = m_sto.write(data, size);
		}

		// ****************************************************
		if (state == FSM::RECORDING_PREP_STOP) {
			if (m_data.spd >= SPD_FLIGHT){
				state = FSM::RECORDING;
			}else if (m_data.spd < SPD_FLIGHT && ms_clock >= 1000 * SPD_FLIGHT_SECS) {
				state = FSM::RECORDING_STOP;
			}
			status = m_sto.write(data, size);
		}

		// ****************************************************
		if (state == FSM::RECORDING_STOP) {
			status = m_sto.close();
			m_recording = false;
			state = FSM::WAIT_SPD;
		}

	}

	return status;
}

// ****************************************************************
void SysControl::system_bit() {

#ifdef IWDG_ENABLE
	// ****************************************************
	// Watch Dog.
	m_sys.wd = bit_watch_dog();
	m_wd.refresh();
#endif

	// ****************************************************
	// Data Storage.
	m_sys.sto = bit_data_storage();
	m_wd.refresh();

	// ****************************************************
	// RTC.
	m_sys.rtc = bit_rtc();
	m_wd.refresh();

	// ****************************************************
	// BT communication.
	m_sys.com = bit_bt_com();
	m_wd.refresh();
}

// ****************************************************************
void SysControl::sensor_bit() {

	uint8_t retries;

	// ****************************************************
	// Temperature sensors.
	retries = 0;
	do {
		m_sen.temp1 = bit_temp(SENSOR::T1);
		m_wd.refresh();
		retries++;
	} while (retries <= BIT_RETRY && m_sen.temp1 != RC::OK);

	retries = 0;
	do {
		m_sen.temp2 = bit_temp(SENSOR::T2);
		m_wd.refresh();
		retries++;
	} while (retries <= BIT_RETRY && m_sen.temp2 != RC::OK);

	// ****************************************************
	// Pressure sensor.
	retries = 0;
	do {
		m_sen.press = bit_pressure();
		m_wd.refresh();
		retries++;
	} while (retries <= BIT_RETRY && m_sen.press != RC::OK);

	// ****************************************************
	// RPM sensor.
	retries = 0;
	do {
		m_sen.rpm = bit_rpm();
		m_wd.refresh();
		retries++;
	} while (retries <= BIT_RETRY && m_sen.rpm != RC::OK);
}

// ****************************************************************
void SysControl::check_cmd(hdr_t hdr, Memory &data) {

	switch (hdr) {

		case HDR::NO_CMD:
			data.bt_valid = false;
			break;

		case HDR::LOC_TIM:
			data.bt_valid = true;
			break;
	}
}

// ****************************************************************
void SysControl::test_gpio() {

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOD);
	GPIO_DeInit(GPIOE);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2
			| GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
			| GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_12
			| GPIO_Pin_14 | GPIO_Pin_9 | GPIO_Pin_7 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12
			| GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_10
			| GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	while (1) {

		GPIO_ToggleBits(GPIOA,
				GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4
						| GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
		GPIO_ToggleBits(GPIOB,
				GPIO_Pin_1 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_12
						| GPIO_Pin_14 | GPIO_Pin_9 | GPIO_Pin_7 | GPIO_Pin_5);
		GPIO_ToggleBits(GPIOC, GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_6);
		GPIO_ToggleBits(GPIOD,
		GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14);
		GPIO_ToggleBits(GPIOE,
		GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14);

		ms_cnt = 1000;
		while (ms_cnt != 0)
			;
	}

}

// ************************************************************************
status_t SysControl::bit_watch_dog() {

	status_t status;

	m_wd.setup(IWDG_TIMEOUT_SEC);

	wd_state_t wd_stat = m_wd.check_for_rst(); // Check for wd introduced reset.

	if (wd_stat == WD_STATE::DID_RST) {
		status = RC::FAIL;
	} else {
		status = RC::OK;
	}

	m_wd.refresh();

	return status;
}

// ************************************************************************
status_t SysControl::bit_rtc() {

	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != RTC_SETUP_WORD) {
		m_rtc.setup();
	}

	RTC_WaitForSynchro();

	return RC::OK;
}

// ************************************************************************
status_t SysControl::bit_data_storage() {

	status_t status = RC::OK;

	m_sto.setup();

	status = m_sto.bit();

	return status;
}

// ************************************************************************
status_t SysControl::bit_temp(SENSOR nr) {

	status_t status = RC::OK;

	// ****************************************************
	// Temp sensor 1.
	if (nr == SENSOR::T1) {

		m_temp.setup(SENSOR::T1);

		m_temp.power_on(SENSOR::T1);

		status = m_temp.bit(SENSOR::T1);

		if (status != RC::OK) {
			m_temp.power_off(SENSOR::T1);
		}
	}

	//****************************************************
	// Temp sensor 2.
	if (nr == SENSOR::T2) {

		m_temp.setup(SENSOR::T2);

		m_temp.power_on(SENSOR::T2);

		status = m_temp.bit(SENSOR::T2);

		if (status != RC::OK) {
			m_temp.power_off(SENSOR::T2);
		}
	}

	return status;
}

// ************************************************************************
status_t SysControl::bit_pressure() {

	status_t status;

	m_mb.setup();

	m_mb.power_on();

	status = m_mb.bit();

	if (status != RC::OK) {
		m_mb.power_off();
	}

	return status;
}

// ************************************************************************
status_t SysControl::bit_rpm() {
	m_rpm.setup();
	return m_rpm.bit();
}

// ************************************************************************
void SysControl::save_start() {

	startup_wait = 1;
	uint32_t cnt = 0;

	while (true) {

		// Check if enough revs have happened to be sure everything is normal.
		if (rpm_cnt > RPM_EM_HALT_WAIT && startup_wait == 1) {
			startup_wait = 0;
			break;
		}

		// Toggle led D3.
		if (cnt++ % 700000 == 0) {
			GPIO_ToggleBits(GPIOA, GPIO_Pin_7);
		}
	}

	// Turn off led ?!? WTF
	GPIO_SetBits(GPIOA, GPIO_Pin_7);
}

// ************************************************************************
status_t SysControl::bit_bt_com() {

	status_t status;

	m_com.setup(BAUD_RATE::AT);
	m_com.set_at_mode(true);
	m_com.power_on();

	status = m_com.bit();

	m_com.set_at_mode(false);

	if (status == RC::OK) {
		m_com.setup(BAUD_RATE::COM);
	} else {
		m_com.power_off();
	}

	return status;
}
// ************************************************************************


