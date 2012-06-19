  #include <iostream>
  #include <string>

  #include <boost/chrono.hpp>

  #include "getmemusage.h"
  #include "block_tracker.hpp"

  namespace chrono = boost::chrono;

  namespace details {
      typedef chrono::system_clock::time_point TimePoint;

      template<typename T>
      void
      memzero(T * foo)
      {
          std::memset(foo, 0, sizeof(*foo));
      }

      struct block_tracker_impl
      {
          TimePoint start_time;
          memory_stats_t start_memory;
        
          double cumulative_time_sec;
        
          // We will abuse the cumulative_memory field a bit. The peak parts
          // will take the obvious value, but we will change the non-peak
          // fields to hold the net increase in memory usage.
          memory_stats_t cumulative_memory;
        
          std::string description;

          bool has_reported;
        
          block_tracker_impl() {
              memzero(&start_memory);
              memzero(&cumulative_time_sec);
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

      pimpl->start_time = chrono::system_clock::now();
      get_self_memory_usage(&pimpl->start_memory);
      pimpl->has_reported = false;
  }

  void
  block_tracker::stop()
  {
      assert(is_running());

      // Update the time
      details::TimePoint end_time = chrono::system_clock::now();
      chrono::duration<double> diff = end_time - pimpl->start_time;

      double sec = diff.count();

      pimpl->cumulative_time_sec += sec;

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

      details::memzero(&pimpl->start_memory);
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
         << "~~~     Cumulative time: " << pimpl->cumulative_time_sec << "\n"
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
      return pimpl->start_memory.resident_bytes_peak != 0;
  }

  block_tracker::~block_tracker() {
      if (is_running()) {
          stop();
      }
      if (!pimpl->has_reported) {
          report();
      }
  }
