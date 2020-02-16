#ifndef BLUETOOTHCONNECTION_H_
#define BLUETOOTHCONNECTION_H_

#include "system/inc/BtCom.h"
#include "Memory.h"

class BluetoothConnection {
public:

	explicit BluetoothConnection(sys::BtCom& com, Memory& memory);

	void send();

private:

	sys::BtCom& m_com;
	Memory& m_memory;

	uint8_t m_data[256];
	uint8_t m_at;

	void begin_send();
	void add_float(float value);
	void add_uint16(uint16_t value);
	void add_uint32(uint32_t value);
	void end_send();
};

#endif /* BLUETOOTHCONNECTION_H_ */
