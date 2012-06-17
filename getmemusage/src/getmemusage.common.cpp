#include "getmemusage.h"

int get_self_vm_bytes(long long * out_bytes)
{
    memory_stats_t stats;
    int err = get_self_memory_usage(&stats);
    *out_bytes = stats.vm_bytes;
    return err;
}

int get_self_vm_bytes_peak(long long * out_bytes)
{
    memory_stats_t stats;
    int err = get_self_memory_usage(&stats);
    *out_bytes = stats.vm_bytes_peak;
    return err;
}

int get_self_resident_bytes(long long * out_bytes)
{
    memory_stats_t stats;
    int err = get_self_memory_usage(&stats);
    *out_bytes = stats.resident_bytes;
    return err;
}

int get_self_resident_bytes_peak(long long * out_bytes)
{
    memory_stats_t stats;
    int err = get_self_memory_usage(&stats);
    *out_bytes = stats.resident_bytes_peak;
    return err;
}
