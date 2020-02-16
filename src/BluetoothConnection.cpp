#include "BluetoothConnection.h"

BluetoothConnection::BluetoothConnection(sys::BtCom& com, Memory& memory)
	: m_com(com)
	, m_memory(memory)
	, m_at(0)
{
}

void BluetoothConnection::send()
{
	begin_send();
	add_float(m_memory.tempCht); // 4
	add_float(m_memory.tempEgt); // 8
	add_float(m_memory.tempAir); // 12
	add_float(m_memory.pressureAir); // 16
	add_uint16(m_memory.rpmMotor); // 18
	add_uint32(m_memory.runtimeMotor); // 22
	end_send();
}

void BluetoothConnection::begin_send()
{
	m_at = 1;
}

void BluetoothConnection::end_send()
{
	m_data[0] = 0xAC;
	m_data[m_at++] = 0xAD;
	m_com.tx_data(m_data, m_at);
}

void BluetoothConnection::add_float(float value)
{
	uint32_t& bytes = reinterpret_cast<uint32_t&>(value);
	m_data[m_at++] = (bytes >> 24) & 0xFF;
	m_data[m_at++] = (bytes >> 16) & 0xFF;
	m_data[m_at++] = (bytes >> 8) & 0xFF;
	m_data[m_at++] = bytes & 0xFF;
}

void BluetoothConnection::add_uint16(uint16_t value)
{
	m_data[m_at++] = (value >> 8) & 0xFF;
	m_data[m_at++] = value & 0xFF;
}

void BluetoothConnection::add_uint32(uint32_t value)
{
	m_data[m_at++] = (value >> 24) & 0xFF;
	m_data[m_at++] = (value >> 16) & 0xFF;
	m_data[m_at++] = (value >> 8) & 0xFF;
	m_data[m_at++] = value & 0xFF;
}
