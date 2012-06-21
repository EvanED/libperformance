#include <functional>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <cassert>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>

#include "getmemusage.h"

extern "C" void annotate(const char * str);


namespace {
    struct DestructionRunner {
        std::function<void ()> callee;

        template<typename Callable>
        DestructionRunner(Callable c)
            : callee(c)
        {}

        ~DestructionRunner() {
            callee();
        }
    };

    const double nsec_to_sec = 1000000000.0;
    const int bytes_to_mb = 1024 * 1024;
    
    void end();
    int outfile_fd = -1;
    timespec start_time;

    std::vector<std::pair<timespec, std::string>> annotations;

    // This function licensced under the MIT license. From
    // http://www.geonius.com/software/libgpl/ts_util.html.
    struct  timespec  tsSubtract (struct  timespec  time1,
                                  struct  timespec  time2)
    {
        struct  timespec  result ;

        if ((time1.tv_sec < time2.tv_sec) ||
            ((time1.tv_sec == time2.tv_sec) &&
             (time1.tv_nsec <= time2.tv_nsec)))
        {
            // TIME1 <= TIME2?
            result.tv_sec = result.tv_nsec = 0 ;
        } else {
            // TIME1 > TIME2
            result.tv_sec = time1.tv_sec - time2.tv_sec ;
            if (time1.tv_nsec < time2.tv_nsec) {
                result.tv_nsec = time1.tv_nsec + 1000000000L - time2.tv_nsec ;
                result.tv_sec-- ;				/* Borrow a second. */
            } else {
                result.tv_nsec = time1.tv_nsec - time2.tv_nsec ;
            }
        }
        return (result) ;
    }
    
    
    void print_prefix()
    {
        std::string s = "data = [\n";
        ssize_t ret = write(outfile_fd, s.c_str(), s.size());
        assert(ret == s.size());
    }

    void print_suffix()
    {
        std::stringstream ss;
        ss << "]\n"
           << "\n"
           << "data_t = zip(*data)\n"
           << "[elapsed, vm_bytes, resident_bytes, vm_bytes_peak, vm_resident_bytes_peak] = data_t\n"
           << "\n"
           << "import matplotlib\n"
           << "matplotlib.use(\"PDF\")\n"
           << "import matplotlib.pyplot as plt\n"
           << "plt.plot(elapsed, vm_bytes_peak, \"k\",\n"
           << "         elapsed, vm_resident_bytes_peak, \"k\",\n"
           << "         elapsed, vm_bytes, \"r\",\n"
           << "         elapsed, resident_bytes, \"b\")\n";

        for (auto const & ann : annotations) {
            double time = ann.first.tv_sec + ann.first.tv_nsec/nsec_to_sec;
            ss << "plt.annotate(\"" << ann.second << "\", "
               << "xy=(" << time << ",1), xytext=(" << time << ",1), "
               << "rotation=\"vertical\", va=\"bottom\", size=\"x-small\")\n";
        }
        
        ss << "plt.savefig(\"blah.pdf\", format=\"pdf\")\n";
        
        std::string s = ss.str();
        ssize_t ret = write(outfile_fd, s.c_str(), s.size());
        assert(ret == s.size());
    }

    void alarm(int x)
    {
        timespec time;
        clock_gettime(CLOCK_REALTIME, &time);
        timespec diff = tsSubtract(time, start_time);
        
        memory_stats_t m;
        get_self_memory_usage(&m);

        std::stringstream ss;
        ss << "    ["
           << diff.tv_sec + diff.tv_nsec/nsec_to_sec << ", "
           << m.vm_bytes/bytes_to_mb << ", "
           << m.resident_bytes/bytes_to_mb << ","       
           << m.vm_bytes_peak/bytes_to_mb << ", "
           << m.resident_bytes_peak/bytes_to_mb << "],\n";

        std::string s = ss.str();
        ssize_t ret = write(outfile_fd, s.c_str(), s.size());
        assert(ret == s.size());
    }

    void start_timer() __attribute__((constructor));    
    void start_timer()
    {
        outfile_fd = creat("blah.py", S_IRUSR | S_IWUSR);
        assert(outfile_fd != -1);
        
        clock_gettime(CLOCK_REALTIME, &start_time);
        
        print_prefix();
        
        struct itimerval tout_val;
        
        signal(SIGALRM, alarm);

        // 0.25sec
        tout_val.it_interval.tv_sec = 0;
        tout_val.it_interval.tv_usec = 250 * 1000;
        tout_val.it_value.tv_sec = 0;
        tout_val.it_value.tv_usec = 250 * 1000;
        setitimer(ITIMER_REAL, &tout_val,0);
    }

    void end()
    {
        signal(SIGVTALRM, SIG_DFL);

        annotate("Program end");

        print_suffix();
    }

    DestructionRunner ender(end);
}

void annotate(const char * str)
{
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    timespec diff = tsSubtract(time, start_time);

    double time_sec = diff.tv_sec + diff.tv_nsec/nsec_to_sec;

    std::stringstream ss;
    ss << str << " (" << time_sec << "sec)";

    annotations.push_back(std::make_pair(diff, std::string(ss.str())));
}



