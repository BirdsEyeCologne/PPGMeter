#include "TaskTemperatureBmp280.h"

namespace task
{

TaskTemperatureBmp280::TaskTemperatureBmp280(Memory& memory)
	: ReccuringTask(250ul, BIT_RETRY)
	, m_memory(memory)
	, m_bmp280()
{
}

bool TaskTemperatureBmp280::init_test()
{
	m_bmp280.setup();
	m_bmp280.power_on();

	const status_t status = m_bmp280.bit();

	if (status != RC::OK)
		m_bmp280.power_off();

	return status == RC::OK;
}

void TaskTemperatureBmp280::task()
{
	if (m_bmp280.get_values(m_memory.pressureAir, m_memory.tempAir) == BMP280_OK)
		return;
	m_memory.pressureAir = 0.f;
	m_memory.tempAir = 0.f;
}

} // namespace task
