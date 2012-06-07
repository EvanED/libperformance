#include <functional>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <cassert>

#include <Windows.h>

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
        std::string s = "data = [\n";
        outfile << s;
        assert(outfile.good());
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
        
        memory_stats_t m;
        get_self_memory_usage(&m);

        std::stringstream ss;
        ss << "    ["
           << diff_sec << ", "
           << m.vm_bytes/bytes_to_mb << ", "
           << m.resident_bytes/bytes_to_mb << ","       
           << m.vm_bytes_peak/bytes_to_mb << ", "
           << m.resident_bytes_peak/bytes_to_mb << "],\n";

        std::string s = ss.str();
        outfile << s;
        assert(outfile.good());
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


