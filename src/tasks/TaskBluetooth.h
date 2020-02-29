#ifndef TASKS_TASKBLUETOOTH_H_
#define TASKS_TASKBLUETOOTH_H_

#include "ReccuringTask.h"

#include <Memory.h>
#include <system/inc/BtCom.h>

namespace task
{

class TaskBluetooth : public ReccuringTask
{
public:

	explicit TaskBluetooth(Memory& memory);

protected:

	void task() override;
	bool init_test() override;

private:

	sys::BtCom m_com;
	Memory& m_memory;

	uint8_t m_data[256];
	uint8_t m_at;

	void begin_send();
	void add_float(float value);
	void add_uint16(uint16_t value);
	void add_uint32(uint32_t value);
	void end_send();

};

} // namespace task

#endif /* TASKS_TASKBLUETOOTH_H_ */
