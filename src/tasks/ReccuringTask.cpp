#include "ReccuringTask.h"

namespace task
{

ReccuringTask::ReccuringTask(uint32_t interval_ms, int init_retries)
	: m_interval(interval_ms)
	, m_init_retries(init_retries)
{
}

ReccuringTask::~ReccuringTask()
{
}

bool ReccuringTask::init_test()
{
	return true;
}

void ReccuringTask::run_test(sys::WatchDog& watchdog)
{
	for(int retries = 0; retries < m_init_retries; ++retries)
	{
		bool result = init_test();
		watchdog.refresh();
		if(result)
			return;
	}
	m_interval.Stop();
}

void ReccuringTask::call()
{
	if(m_interval.Triggered())
		task();
}

} // namespace task
