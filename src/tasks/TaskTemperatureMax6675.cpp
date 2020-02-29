#include "TaskTemperatureMax6675.h"

namespace task
{

TaskTemperatureMax6675::TaskTemperatureMax6675(Memory& memory)
	: ReccuringTask(1000ul, BIT_RETRY)
	, m_memory(memory)
	, m_max6675()
	, m_ok_cht(false)
	, m_ok_egt(false)
{
}

bool TaskTemperatureMax6675::sensor_ok(SENSOR num)
{
	m_max6675.setup(num);
	m_max6675.power_on(num);

	const status_t status = m_max6675.bit(num);

	if (status != RC::OK)
		m_max6675.power_off(num);

	return status == RC::OK;
} 

bool TaskTemperatureMax6675::init_test()
{
	m_ok_cht = sensor_ok(SENSOR::T1);
	m_ok_egt = sensor_ok(SENSOR::T2);
	return m_ok_cht || m_ok_egt;
}

void TaskTemperatureMax6675::task()
{
	if (m_ok_cht)
		m_memory.tempCht = m_max6675.get_value(SENSOR::T1);

	if (m_ok_egt)
		m_memory.tempEgt = m_max6675.get_value(SENSOR::T2);
}

} // namespace task
