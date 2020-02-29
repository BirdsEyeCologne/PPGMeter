/*
 * Control.cpp
 *
 *  Created on: Sep 7, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <SysControl.h>

#include <Setting.h>

#include <tasks/TaskBluetooth.h>
#include <tasks/TaskTemperatureMax6675.h>
#include <tasks/TaskTemperatureBmp280.h>
#include <tasks/TaskRpm.h>
#include <tasks/TaskLogging.h>
#include <tasks/TaskClockSync.h>

// From stm32f4xx_it.c -> TIM2_IRQHandler
extern volatile uint32_t rpm_cnt;

// For stm32f4xx_it.c
volatile uint32_t ms_clock;

// ****************************************************************
SysControl::SysControl()
{

	// Init stuff.
	setup();

	// Startup Built In Test (BIT) of system components
	// and sensors.
	startup_bit();
}

// ****************************************************************
SysControl::~SysControl() {}

// ****************************************************************
void SysControl::setup() {

	ms_clock = 0; // Just in case ;)

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
	// TODO: Currently uc is never in standby mode. Previous behaviour was standby if rpm == 0 and not recording,
	// but we might want temperature etc. when motor is off
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

	// Settings
	Setting::Add<int>("sparks_per_revolution", "sparks", 1);
	Setting::Load();

	// Define tasks
	m_tasks.emplace_back(std::make_unique<task::TaskBluetooth>(m_memory));
	m_tasks.emplace_back(std::make_unique<task::TaskTemperatureMax6675>(m_memory));
	m_tasks.emplace_back(std::make_unique<task::TaskTemperatureBmp280>(m_memory));
	m_tasks.emplace_back(std::make_unique<task::TaskRpm>(m_memory, m_rtc));
	m_tasks.emplace_back(std::make_unique<task::TaskLogging>(m_memory, m_rtc, m_led));
	m_tasks.emplace_back(std::make_unique<task::TaskClockSync>(m_memory, m_rtc));
}

// ****************************************************************
void SysControl::run() {

	while (true)
	{
		// System components
		for(const auto& task : m_tasks)
			task->call();
		m_wd.refresh();
	}
}

// ****************************************************************
void SysControl::startup_bit() {

	#ifdef IWDG_ENABLE
	// ****************************************************
	// Watch Dog.
	m_sys_bit.wd = bit_watch_dog();
	m_wd.refresh();
	#endif

	// RTC.
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != RTC_SETUP_WORD)
		m_rtc.setup_once();
	RTC_WaitForSynchro();
	m_wd.refresh();

	// System components bit.
	for(const auto& task : m_tasks)
		task->run_test(m_wd);
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

		Interval::blocking_wait(1000ul);
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

#ifdef false

// ************************************************************************
// To be sure the motor is porplerly working, a minimum of RPM_EM_HALT_WAIT revs have to
// be detected, without to high RPMs. The actual check of to high RPMs is done
// in TIM2_IRQHandler (stm32f4xx_it.c).
// check_rpm will get FALSE if the RPMs are to high during save start (set in TIM2_IRQHandler).
// check_rpm get also set FALSE if save start is ok, this will disable checking
// of to high RPMs in TIM2_IRQHandler (disabling save start).
// This method will return only, if save start is done and no high RPMs are detected.
void SysControl::save_start() {

	check_rpm = TRUE;
	uint32_t cnt = 0;

	while (true) {

		if (rpm_cnt > RPM_EM_HALT_WAIT && check_rpm == TRUE) {
			check_rpm = FALSE;
			break;
		}

		// Toggle led D2 to indicate a save start halt of the motor.
		if (check_rpm == FALSE && cnt % 700000 == 0) {
			m_led.toggle(LED::D2);
		}

		if (cnt % 700000 == 0) {
			m_led.toggle(LED::D3);
		}

		cnt++;
	}

	// Configure Emergency Halt pin as input, just in case...
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	rpm_cnt = 0;
	ms_clock = 0;

	m_led.off(LED::D3);

}

#endif
