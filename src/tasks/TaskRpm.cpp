#include "TaskRpm.h"

namespace task
{

TaskRpm::TaskRpm(Memory& memory, sys::Clock& rtc)
	: ReccuringTask(1000ul, BIT_RETRY)
	, m_memory(memory)
	, m_rtc(rtc)
	, m_rpm()
	, m_spinning(false)
{
	// Setup of rpm interrupts etc. needed for save_start / em halt!
	m_rpm.setup();
}

bool TaskRpm::init_test()
{
	m_rpm.setup();
	return m_rpm.bit() == RC::OK;
}

void TaskRpm::task()
{
	m_memory.rpmMotor = m_rpm.get_value() * RPM_SCALE;

	// Trigger total time measuring of the motor spinning (motor hour meter).
	if (m_memory.rpmMotor != 0 && m_spinning == false)
	{
		m_rtc.measure_start();
		m_spinning = true;
	}
	else if (m_memory.rpmMotor == 0 && m_spinning == true)
	{
		m_rtc.measure_stop();
		m_spinning = false;
	}

	m_memory.runtimeMotor = m_rtc.get_measure();
}

} // namespace task
