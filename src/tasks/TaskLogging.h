#ifndef TASKS_TASKLOGGING_H_
#define TASKS_TASKLOGGING_H_

#include "ReccuringTask.h"
#include <Memory.h>
#include <system/inc/DataStorage.h>
#include <system/inc/Clock.h>
#include <system/inc/Led.h>

namespace task
{

class TaskLogging : public ReccuringTask
{
public:

	explicit TaskLogging(Memory& memory, sys::Clock& rtc, sys::Led& led);

protected:

	void task() override;
	bool init_test() override;

private:

	// Stores interesting values of m_memory onto SD-Card. Recording is only done, when a takeoff is
	// reccognized and durig flight. Both, takeoff and flight depends on certain speeds and
	// times during the speeds are present.
	status_t record(uint8_t * data, uint8_t size); // TODO

	Memory& m_memory;
	sys::Clock& m_rtc;
	sys::Led& m_led;

	sys::DataStorage m_sto;
	bool storage_ok;
	bool m_recording;
	Interval m_flight_takeoff;
	Interval m_flight_landing;

};

} // namespace task

#endif /* TASKS_TASKLOGGING_H_ */
