#ifndef TASKS_TASKTEMPERATUREBMP280_H_
#define TASKS_TASKTEMPERATUREBMP280_H_

#include "ReccuringTask.h"
#include <sensor/inc/Pressure.h>
#include <Memory.h>

namespace task
{

class TaskTemperatureBmp280 : public ReccuringTask
{
public:

	explicit TaskTemperatureBmp280(Memory& memory);

protected:

	void task() override;
	bool init_test() override;

private:

	Memory& m_memory;
	sens::Pressure m_bmp280;
};

} // namespace task

#endif /* TASKS_TASKTEMPERATUREBMP280_H_ */
