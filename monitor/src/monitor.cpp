#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

#include "getmemusage.h"
#include "monitor.h"

namespace chrono = boost::chrono;

using boost::adaptors::transformed;
using boost::thread;


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

    const int bytes_to_mb = 1024 * 1024;

    void end();
    
    bool start_time_set = false;
    chrono::system_clock::time_point start_time;

    double elapsed_sec()
    {
        assert(start_time_set);
        chrono::duration<double> diff = chrono::system_clock::now() - start_time;
        return diff.count();
    }

    typedef std::pair<double, std::string> Annotation;
    typedef std::vector<Annotation> annotation_list;
    annotation_list annotations;

    std::string stringize_tracker(Tracker & t)
    {
        std::stringstream ss;
        ss << "\"" << t.second << "\": " << t.first();
        return ss.str();
    }

    std::string stringize_annotation(Annotation & t)
    {
        std::stringstream ss;
        ss << "{ \"time_offset_sec\": " << t.first << ", "
           << "\"annotation\": \"" << t.second << "\"}";
        return ss.str();
    }

    void alarm()
    {
        std::stringstream ss;
        ss << "  { ";
        ss << "\"time_offset_sec\": " << elapsed_sec() << ", ";
        ss << boost::join(trackers | transformed(stringize_tracker), ", ");
        ss << "},";

        std::string const & str = ss.str();
        char const * cstr = str.c_str();

        // std::vector<Callback> callbacks;
        std::for_each(callbacks.begin(), callbacks.end(),
                      [cstr](Callback & cb) {
                          cb(cstr);
                     });
    }

    volatile bool gogogo = true;

    void loop_the_loop()
    {
        std::for_each(callbacks.begin(), callbacks.end(),
                      [](Callback & cb) {
                          cb("{\"measurements\": [");
                      });
        while(gogogo) {
          thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(250));
          alarm();
        }
        std::for_each(callbacks.begin(), callbacks.end(),
                      [](Callback & cb) {
                          cb("],\n\"annotations\": [");
                      });
        std::string all_annots_str = boost::join(annotations | transformed(stringize_annotation), ",\n  ");
        char const * all_annots_cstr = all_annots_str.c_str();
        std::for_each(callbacks.begin(), callbacks.end(),
                      [all_annots_cstr](Callback & cb) {
                          cb(all_annots_cstr);
                      });
        std::for_each(callbacks.begin(), callbacks.end(),
                      [](Callback & cb) {
                          cb("]}");
                      });
    }

    thread loop_thread;

    void start()
    {
        start_time_set = true;
        start_time = chrono::system_clock::now();

        // For testing purposes
        register_tracker_error_returner(get_self_vm_bytes, "VM bytes");
        //register_tracker_error_returner(get_self_resident_bytes, "RSS bytes");
        //register_tracker_error_returner(get_self_vm_bytes_peak, "VM bytes peak");
        //register_tracker_error_returner(get_self_resident_bytes_peak, "RSS bytes peak");

        register_monitor_callback([](char const * str) -> void { std::cout << str << "\n"; });

        loop_thread = thread(loop_the_loop);
    }

    void end()
    {
        gogogo = false;

        loop_thread.join();
    }

    ConstructionRunner starter(start);
    DestructionRunner ender(end);
}

void annotate(const char * str)
{
    annotations.push_back(std::make_pair(elapsed_sec(), std::string(str)));
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

void register_tracker(std::function<long long ()> const & tracker, std::string const & key)
{
    trackers.push_back(Tracker(tracker, key));
}

void register_monitor_callback(std::function<void (char const *)> const & callback)
{
    callbacks.push_back(callback);
}
