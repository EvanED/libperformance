#include "getmemusage.h"

#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include <cstring>
#include <cassert>

#include <Windows.h>
#include <psapi.h>

using namespace std;

#pragma comment(lib, "psapi.lib")

namespace {
    int get_memory_usage(HANDLE process, memory_stats_t * out_stats)
    {
      PROCESS_MEMORY_COUNTERS meminfo;
      BOOL ret = GetProcessMemoryInfo(process, &meminfo, sizeof(meminfo));
      assert(ret);

      out_stats->vm_bytes = meminfo.PagefileUsage;
      out_stats->vm_bytes_peak = meminfo.PeakPagefileUsage;
      out_stats->resident_bytes = meminfo.WorkingSetSize;
      out_stats->resident_bytes_peak = meminfo.PeakWorkingSetSize;

      return 0;
    }
}


int get_self_memory_usage(memory_stats_t * out_stats)
{
    return get_memory_usage(GetCurrentProcess(), out_stats);
}

    
int get_process_memory_usage(int pid, memory_stats_t * out_stats)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, false, pid);
    assert(process != NULL);
    
    return get_memory_usage(process, out_stats);
}


