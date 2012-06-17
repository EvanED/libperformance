#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <cassert>

#include <Windows.h>

#include "getmemusage.h"
#include "boost/scoped_ptr.hpp"

#include "../../../monitor/src/monitor.h"


namespace {
    struct ErrorReturnerWrapper {
        error_returner_t const func;

        long long operator()() {
            long long out;
            int err = func(&out);
            if (err) {
                throw "Error!";
            }
            return out;
        }

        ErrorReturnerWrapper(error_returner_t f)
            : func(f)
        {}

    private:
        void operator=(ErrorReturnerWrapper const & other);
    };

    typedef std::pair<std::function<long long()>, std::string> Tracker;
    typedef std::function<void (char const*)> Callback;

    std::vector<Tracker> trackers;
    std::vector<std::function<void (char const*)>> callbacks;

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

    struct ConstructionRunner {
        template<typename Callable>
        ConstructionRunner(Callable callee)
        {
          callee();
        }
    };

    const double nsec_to_sec = 1000000000.0;
    const int bytes_to_mb = 1024 * 1024;

    std::ofstream outfile("blah.py");
    
    void end();
    int outfile_fd = -1;
    LARGE_INTEGER start_time;

    typedef std::vector<std::pair<double, std::string>> annotation_list;
    annotation_list annotations;

    void print_prefix()
    {
        std::string s = "[\n";
        outfile << s;
        assert(outfile.good());
    }

    void print_suffix()
    {
        std::stringstream ss;
        ss << "]\n";

        for (annotation_list::const_iterator it = annotations.begin();
             it != annotations.end(); ++it) 
        {
            auto const & ann = *it;
            double time = ann.first;
            ss << "plt.annotate(\"" << ann.second << "\", "
               << "xy=(" << time << ",1), xytext=(" << time << ",1), "
               << "rotation=\"vertical\", va=\"bottom\", size=\"x-small\")\n";
        }
        
        ss << "plt.savefig(\"blah.pdf\", format=\"pdf\")\n";
        
        std::string s = ss.str();
        outfile << s;
        assert(outfile.good());
    }

    void alarm()
    {
        LARGE_INTEGER time;
        BOOL ret = QueryPerformanceCounter(&time);
        assert(ret);

        LARGE_INTEGER freq;
        ret = QueryPerformanceFrequency(&freq);
        assert(ret);

        double diff_sec = (time.QuadPart - start_time.QuadPart)/static_cast<double>(freq.QuadPart);
        
        std::stringstream ss;
        ss << "{ ";
        ss << "\"time_offset_sec\": " << diff_sec << ", ";
        //std::vector<Tracker> trackers;
        std::for_each(trackers.begin(), trackers.end(),
                      [&ss](Tracker & t) {
                          ss << "\"" << t.second << "\": " << t.first() << ", ";
                      });

        ss << "}";

        std::string const & str = ss.str();
        char const * cstr = str.c_str();

        // std::vector<Callback> callbacks;
        std::for_each(callbacks.begin(), callbacks.end(),
                      [cstr](Callback & cb) {
                          cb(cstr);
                     });
    }

    volatile bool gogogo = true;

    DWORD __stdcall loop_the_loop(LPVOID)
    {
        assert(outfile.good());
        
        BOOL ret = QueryPerformanceCounter(&start_time);
        assert(ret);
        
        print_prefix();
        
        while(gogogo) {
          Sleep(250);
          alarm();
        }

        print_suffix();

        return 0;
    }

    HANDLE loop_thread;

    void start()
    {
      loop_thread = CreateThread(NULL, 0, loop_the_loop, 0, 0, NULL);
      assert(loop_thread != NULL);
    }

    void end()
    {
        gogogo = false;

        DWORD ret = WaitForSingleObject(loop_thread, INFINITE);
        assert(ret == WAIT_OBJECT_0);
    }

    ConstructionRunner starter(start);
    DestructionRunner ender(end);
}

void annotate(const char * str)
{
    LARGE_INTEGER time;
    BOOL ret = QueryPerformanceCounter(&time);
    assert(ret);

    LARGE_INTEGER freq;
    ret = QueryPerformanceFrequency(&freq);
    assert(ret);

    double diff_sec = (time.QuadPart - start_time.QuadPart)/static_cast<double>(freq.QuadPart);

    annotations.push_back(std::make_pair(diff_sec, std::string(str)));
}

void register_tracker_error_returner(error_returner_t tracker, char const * key)
{
    trackers.push_back(Tracker(ErrorReturnerWrapper(tracker), key));
}

void register_tracker_value_returner(value_returner_t tracker, char const * key)
{
    trackers.push_back(Tracker(tracker, key));
}

void register_monitor_callback(monitor_callback_t callback)
{
    callbacks.push_back(callback);
}
