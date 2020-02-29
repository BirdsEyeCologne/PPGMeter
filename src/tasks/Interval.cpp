#include "Interval.h"

// For stm32f4xx_it.c
extern volatile uint32_t ms_clock;

Interval::Interval(uint32_t interval_ms, bool stopped, bool immediate)
    : m_interval(interval_ms)
    , m_stopped(stopped)
{
    if(!m_stopped)
    {
        if(immediate)
        {
            m_last_time =  ms_clock - m_interval;
        }
        else
            Reset();
    }
}

void Interval::blocking_wait(uint32_t interval_ms)
{
    Interval interval(interval_ms);
    while(!interval.Triggered());
}

bool Interval::Triggered()
{
    if(m_stopped)
        return false;

    uint32_t curTime = ms_clock;
    if((curTime - m_last_time) >= m_interval)
    {
        m_last_time = curTime;
        if(m_interval > 0)
            m_last_time -= (curTime % m_interval);
        return true;
    }
    return false;
}

bool Interval::Stopped()
{
    return m_stopped;
}

void Interval::Reset()
{
    m_last_time = ms_clock;
    m_stopped = false;
}

void Interval::Stop()
{
    m_stopped = true;
}

float Interval::Progress() const
{
	if(m_stopped)
		return 1.0f;

	const unsigned long curTime = ms_clock;
	const unsigned long left = m_last_time + m_interval - curTime;
	return std::min(1.0f, std::max(0.0f, 1.0f - float(left) / float(m_interval)));
}

unsigned long Interval::GetInterval() const
{
    return m_interval;
}
