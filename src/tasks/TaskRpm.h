#ifndef TASKS_TASKRPM_H_
#define TASKS_TASKRPM_H_

#include "ReccuringTask.h"
#include <Memory.h>
#include <sensor/inc/RPM.h>
#include <system/inc/Clock.h>

namespace task
{

class TaskRpm : public ReccuringTask
{
public:

	explicit TaskRpm(Memory& memory, sys::Clock& rtc);

protected:

	void task() override;
	bool init_test() override;

private:

	Memory& m_memory;
	sys::Clock& m_rtc;
	sens::RPM m_rpm;

	bool m_spinning;

};

} // namespace task

#endif /* TASKS_TASKRPM_H_ */
