#ifndef TASKS_TASKCLOCKSYNC_H_
#define TASKS_TASKCLOCKSYNC_H_

#include "ReccuringTask.h"
#include <Memory.h>
#include <system/inc/Clock.h>

namespace task
{

class TaskClockSync : public ReccuringTask
{
public:

	explicit TaskClockSync(Memory& memory, sys::Clock& rtc);

protected:

	void task() override;

private:

	Memory& m_memory;
	sys::Clock& m_rtc;

};

} // namespace task

#endif /* TASKS_TASKCLOCKSYNC_H_ */
