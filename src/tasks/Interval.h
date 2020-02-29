#ifndef INTERVAL_H_
#define INTERVAL_H_

#include <memory>

/// Timer that is triggered in a regular interval
class Interval
{
public:

    /** A new interval needs an interval in which it should get trigger,
     * it can be initial started (default) or stopped
     * and the first trigger can be immediately or after the first interval (default).
     * global_clock must be monotonic, overflows are accounted for.
     **/
    Interval(uint32_t interval_ms, bool stopped = false, bool immediate = false);

    /// Create a temporary Interval and blocking wait until it triggered once
    static void blocking_wait(uint32_t interval_ms);

    /// Is a trigger due?
    bool Triggered();

    /// Was the trigger stopped?
    bool Stopped();

    /// Start measuring time, next trigger will be in interval ms
    void Reset();

    /// Stopp regular triggering
    void Stop();

    /// Progress (0 to 1) of the interval, will trigger if 1 is reached
    float Progress() const;

    /// Get interval that was given in constructor
    unsigned long GetInterval() const;

private:

    const uint32_t m_interval;

    uint32_t m_last_time;
    bool m_stopped;

};

#endif /* INTERVAL_H_ */
