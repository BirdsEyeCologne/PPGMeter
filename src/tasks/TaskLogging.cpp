#include "TaskLogging.h"

namespace task
{

TaskLogging::TaskLogging(Memory& memory, sys::Clock& rtc, sys::Led& led)
	: ReccuringTask(1000ul, BIT_RETRY)
	, m_memory(memory)
	, m_rtc(rtc)
	, m_led(led)
	, m_sto()
	, storage_ok(false)
	, m_recording(false)
	, m_flight_takeoff(1000ul * SPD_TAKEOFF_SECS, true)
	, m_flight_landing(1000ul * SPD_FLIGHT_SECS, true)
{
}

bool TaskLogging::init_test()
{
	m_sto.setup();
	storage_ok = m_sto.bit() == RC::OK;
	return true; // Allways running to control led, even if not storage_ok
}

void TaskLogging::task()
{
	if (m_recording == true)
		m_led.on(LED::D2);				// Recording.
	else
		m_led.toggle(LED::D2);			// Flash LED as alive indicator :)

	if(!storage_ok)
		return;

	// Get the values of the local RTC, not the values
	// stored on m_memory, which is the smartphone time.
	// Most likely both times are equal. Regular synch of
	// uc clock is recomended.
	RTC_TimeTypeDef now = m_rtc.get_time();

	m_memory.sec = now.RTC_Seconds;// Dirty hack, see Memory.h (sec and day)
	m_memory.min = now.RTC_Minutes;
	m_memory.hour = now.RTC_Hours;

	// Record sensor data to the SD-Card memory.
	// Recording is based on speed and time (takeoff/flight/landed).
	// TODO status_t status = record(m_memory.recv.data(), 31);

	// If to many recording errors have happened, disable storage.
	/* TODO if (status == RC::ERROR) {
		static uint8_t error = 0;
		if (error++ >= SD_ERROR_DISABLE) {
			m_bit.sto = RC::FAIL;
		}
	}*/

	// Invalidate the data. Only freshly received
	// data should be stored onto SD-Card.
	m_memory.bt_valid = false;
}


// ****************************************************************
status_t TaskLogging::record(uint8_t * data, uint8_t size)
{
	status_t status = RC::OK;

	// Check if valid data is received from smartphone. If valid data is
	// present, analyze to decide if it should be stored on SD-Card.
	if (m_memory.bt_valid != true)
		return status;

	if (!m_recording)
	{
		if(m_memory.spd >= SPD_TAKEOFF)
		{
			if (m_flight_takeoff.Stopped())
				m_flight_takeoff.Reset();

			if (m_flight_takeoff.Triggered())
			{
				// Start recording
				RTC_DateTypeDef today = m_rtc.get_date();
				status = m_sto.open(today);
				m_recording = true;
				m_flight_takeoff.Stop();
			}
		}
		else
		{
			m_flight_takeoff.Stop();
		}
	}
	else
	{
		status = m_sto.write(data, size);

		if (m_memory.spd < SPD_FLIGHT && m_flight_landing.Stopped())
			m_flight_landing.Reset();
		

		if(m_memory.spd < SPD_FLIGHT)
		{
			if (m_flight_landing.Stopped())
				m_flight_landing.Reset();

			if (m_flight_landing.Triggered())
			{
				// Stop recording
				status = m_sto.close();
				m_recording = false;
				m_flight_landing.Stop();
			}
		}
		else
		{
			m_flight_landing.Stop();
		}
		
	}

	return status;
}

} // namespace task
