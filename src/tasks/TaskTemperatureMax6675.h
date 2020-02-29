#ifndef TASKS_TASKTEMPERATUREMAX6675_H_
#define TASKS_TASKTEMPERATUREMAX6675_H_

#include "ReccuringTask.h"
#include <sensor/inc/Temp.h>
#include <Memory.h>

namespace task
{

class TaskTemperatureMax6675 : public ReccuringTask
{
public:

	explicit TaskTemperatureMax6675(Memory& memory);

protected:

	void task() override;
	bool init_test() override;

private:

	bool sensor_ok(SENSOR num);

	Memory& m_memory;
	sens::Temp m_max6675;
	bool m_ok_cht;
	bool m_ok_egt;

};

} // namespace task

#endif /* TASKS_TASKTEMPERATUREMAX6675_H_ */
