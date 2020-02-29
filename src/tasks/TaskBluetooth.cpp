#include "TaskBluetooth.h"

namespace task
{

TaskBluetooth::TaskBluetooth(Memory& memory)
	: ReccuringTask(250ul, 1)
	, m_com()
	, m_memory(memory)
	, m_at(0)
{
}

bool TaskBluetooth::init_test()
{
	m_com.setup(BAUD_RATE::AT);
	m_com.set_at_mode(true);
	m_com.power_on();

	const status_t status = m_com.bit();

	m_com.set_at_mode(false);

	if (status == RC::OK) {
		m_com.setup(BAUD_RATE::COM);
	} else {
		m_com.power_off();
	}

	return status == RC::OK;
}

void TaskBluetooth::task()
{
	// Transmit sensor values to smartphone.
	begin_send();
	add_float(m_memory.tempCht); // 4
	add_float(m_memory.tempEgt); // 8
	add_float(m_memory.tempAir); // 12
	add_float(m_memory.pressureAir); // 16
	add_uint16(m_memory.rpmMotor); // 18
	add_uint32(m_memory.runtimeMotor); // 22
	end_send();

	// Receive sensor values from smartphone.
	hdr_t hdr = m_com.rx_data(m_memory.recv.data());

	// Check received command by header information.
	switch (hdr)
	{
	case HDR::NO_CMD:
		m_memory.bt_valid = false;
		break;
	case HDR::LOC_TIM:
		m_memory.bt_valid = true;
		break;
	}

	#ifdef false
		// Simulation of takeoff, flight and landing :)
		static uint32_t cnt = 0;
		cnt++;
		if(cnt >= 20 && cnt <= 60 && m_memory.bt_valid == true){
			m_memory.spd = 40;
		}

		if(cnt >= 90 && cnt <= 120 && m_memory.bt_valid == true){
			m_memory.spd = 35;
		}
	#endif
}

void TaskBluetooth::begin_send()
{
	m_at = 1;
}

void TaskBluetooth::end_send()
{
	m_data[0] = 0xAC;
	m_data[m_at++] = 0xAD;
	m_com.tx_data(m_data, m_at);
}

void TaskBluetooth::add_float(float value)
{
	uint32_t& bytes = reinterpret_cast<uint32_t&>(value);
	m_data[m_at++] = (bytes >> 24) & 0xFF;
	m_data[m_at++] = (bytes >> 16) & 0xFF;
	m_data[m_at++] = (bytes >> 8) & 0xFF;
	m_data[m_at++] = bytes & 0xFF;
}

void TaskBluetooth::add_uint16(uint16_t value)
{
	m_data[m_at++] = (value >> 8) & 0xFF;
	m_data[m_at++] = value & 0xFF;
}

void TaskBluetooth::add_uint32(uint32_t value)
{
	m_data[m_at++] = (value >> 24) & 0xFF;
	m_data[m_at++] = (value >> 16) & 0xFF;
	m_data[m_at++] = (value >> 8) & 0xFF;
	m_data[m_at++] = value & 0xFF;
}

} // namespace task
