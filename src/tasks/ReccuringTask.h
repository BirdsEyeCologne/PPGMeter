#ifndef RECCURINGTASK_H_
#define RECCURINGTASK_H_

#include <functional>
#include "Interval.h"
#include <system/inc/WatchDog.h>

namespace task
{

/// A task that is executed every x milliseconds, inherit to define your tasks
class ReccuringTask
{
protected:

	/// Override this function to define your task
	virtual void task() = 0;

	/// Optionally override this function to define an initial test that is performed when run_test is called.
	/** 
	 * If the test fails (returns false), the task will never be executed.
	 * If not overriden will return true.
	 **/
	virtual bool init_test();

public:

	ReccuringTask(uint32_t interval_ms, int init_retries);
	virtual ~ReccuringTask();

	/// Call init_test to decide if task should be executed
	void run_test(sys::WatchDog& watchdog);

	/// Check if interval has passed and if so, execute task
	void call();

private:

	Interval m_interval;
	int m_init_retries;

};

} // namespace task

#endif /* RECCURINGTASK_H_ */
