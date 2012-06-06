#include "getmemusage.h"

#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include <cstring>
#include <cassert>

#include <sys/types.h>
#include <unistd.h>

#include <boost/algorithm/string/predicate.hpp>

using namespace std;
using boost::algorithm::starts_with;
using boost::algorithm::ends_with;

namespace {
    vector<string>
    readlines(istream & is)
    {
        string s;
        vector<string> v;
        
        assert(is.good());
        while(getline(is, s)) {
            v.push_back(s);
        }

        return v;
    }

    vector<string>
    readlines_file(string const & filename)
    {
        ifstream file(filename.c_str());
        return readlines(file);
    }


    long long
    extract_bytes_from_line(string const & line)
    {
        assert(ends_with(line, "kB"));

        string dummy;
        long long value = 0;

        stringstream ss(line);
        ss >> dummy >> value;

        if (!ss.good()) {
            cerr << "ERROR: cannot parse line " << line << "\n"
                 << "    read dummy " << dummy << "\n"
                 << "    read value " << value << "\n";
            assert(false);
        }

        return value * 1024;
    }
}


int get_self_memory_usage(memory_stats_t * out_stats)
{
    return get_process_memory_usage(getpid(), out_stats);
}

    
int get_process_memory_usage(int pid, memory_stats_t * out_stats)
{
    memset(static_cast<void*>(out_stats), 0, sizeof(*out_stats));
    
    stringstream ss;
    ss << "/proc/" << pid << "/status";
   
    auto const & lines = readlines_file(ss.str());
    for (string const & line : lines) {
        if (starts_with(line, "VmSize:")) {
            assert(out_stats->vm_bytes == 0);
            out_stats->vm_bytes = extract_bytes_from_line(line);
        }
        else if (starts_with(line, "VmPeak:")) {
            assert(out_stats->vm_bytes_peak == 0);
            out_stats->vm_bytes_peak = extract_bytes_from_line(line);
        }
        else if (starts_with(line, "VmRSS:")) {
            assert(out_stats->resident_bytes == 0);
            out_stats->resident_bytes = extract_bytes_from_line(line);
        }
        else if (starts_with(line, "VmHWM:")) {
            // HWM = "high water mark"
            assert(out_stats->resident_bytes_peak == 0);
            out_stats->resident_bytes_peak = extract_bytes_from_line(line);
        }
    }
    
    
    return 0;
}


