#include "TaskClockSync.h"

volatile uint8_t time_sync;

namespace task
{

TaskClockSync::TaskClockSync(Memory& memory, sys::Clock& rtc)
	: ReccuringTask(1000ul, BIT_RETRY)
	, m_memory(memory)
	, m_rtc(rtc)
{
	// Init external stuff.
	time_sync = 0;
}

void TaskClockSync::task()
{
	// Sync time with smartphone **********************
	if (time_sync == 1) { // User has pressed BTN::K0.
		if (m_memory.bt_valid == true) {

			// Use the data stored on m_memory, which has been sent
			// via bluetooth from the smartphone.
			RTC_TimeTypeDef now = { m_memory.hour, m_memory.min, 0, 0 };
			RTC_DateTypeDef today = { 0, m_memory.month, m_memory.day, m_memory.year };

			m_rtc.set_time(now);
			m_rtc.set_date(today);
		}
		time_sync = 0;
	}
}

} // namespace task