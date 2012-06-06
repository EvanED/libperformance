#include <iostream>
#include <string>

#include <cstring>

#include <sys/types.h>
#include <sys/time.h>

#include "getmemusage.h"
#include "block_tracker.hpp"

namespace {
    inline unsigned long long timevaldiff(struct timeval *starttime, struct timeval *finishtime)
    {
        unsigned long long msec;
        msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
        msec+=(finishtime->tv_usec-starttime->tv_usec)/1000;
        return msec;
    }

    template<typename T>
    void
    memzero(T * p)
    {
        memset(static_cast<void*>(p), 0, sizeof(*p));
    }
}


namespace details {
    struct block_tracker_impl
    {
        timeval start_time;
        memory_stats_t start_memory;
        
        long long cumulative_time_ms;
        
        // We will abuse the cumulative_memory field a bit. The peak parts
        // will take the obvious value, but we will change the non-peak
        // fields to hold the net increase in memory usage.
        memory_stats_t cumulative_memory;
        
        std::string description;

        bool has_reported;
        
        block_tracker_impl() {
            memzero(&start_time);
            memzero(&start_memory);
            memzero(&cumulative_time_ms);
            memzero(&cumulative_memory);
            has_reported = true;
        }
    };
}


block_tracker::block_tracker()
    : pimpl(new details::block_tracker_impl)
{
    start();
}
    
block_tracker::block_tracker(std::string const & desc)
    : pimpl(new details::block_tracker_impl)
{
    pimpl->description = desc;
    start();
}

void
block_tracker::start()
{
    assert(!is_running());

    gettimeofday(&pimpl->start_time, NULL);
    get_self_memory_usage(&pimpl->start_memory);

    pimpl->has_reported = false;
}

void
block_tracker::stop()
{
    assert(is_running());

    // Update the time
    timeval end_time;
    gettimeofday(&end_time, NULL);
    unsigned long long ms = timevaldiff(&pimpl->start_time, &end_time);
    pimpl->cumulative_time_ms += ms;

    // Update the memory
    memory_stats_t end_mem;
    get_self_memory_usage(&end_mem);
    // Peaks get the max, which should be what we see now
    assert(pimpl->cumulative_memory.vm_bytes_peak <= end_mem.vm_bytes_peak);
    assert(pimpl->cumulative_memory.resident_bytes_peak <= end_mem.resident_bytes_peak);
    pimpl->cumulative_memory.vm_bytes_peak = end_mem.vm_bytes_peak;
    pimpl->cumulative_memory.resident_bytes_peak = end_mem.resident_bytes_peak;
    // Others get the net effect
    pimpl->cumulative_memory.vm_bytes += (end_mem.vm_bytes - pimpl->start_memory.vm_bytes);
    pimpl->cumulative_memory.resident_bytes += (end_mem.resident_bytes - pimpl->start_memory.resident_bytes);

    memzero(&pimpl->start_time);
    memzero(&pimpl->start_memory);
}

void
block_tracker::report() const
{
    report(std::cerr);
}

void
block_tracker::report(std::ostream & os) const
{
    assert(!is_running());
    os << "~~~ Resource usage tracker " << pimpl->description << "\n"
       << "~~~     Cumulative time: " << pimpl->cumulative_time_ms/1000.0 << "\n"
       << "~~~     Net increase in VM size: " << pimpl->cumulative_memory.vm_bytes << "\n"
       << "~~~     Net increase in RS size: " << pimpl->cumulative_memory.resident_bytes << "\n"
       << "~~~     Peak VM size: " << pimpl->cumulative_memory.vm_bytes_peak << "\n"
       << "~~~     Peak RS size: " << pimpl->cumulative_memory.resident_bytes_peak << "\n"
       << "~~~ (Note: the peak VM and RS sizes may not have been achieved during\n"
       << "~~~ the duration of this resource tracker)\n";
    pimpl->has_reported = true;
}

bool
block_tracker::is_running() const
{
    // Could make this more robust.
    return pimpl->start_time.tv_sec != 0;
}

block_tracker::~block_tracker() {
    if (is_running()) {
        stop();
    }
    if (!pimpl->has_reported) {
        report();
    }
}
