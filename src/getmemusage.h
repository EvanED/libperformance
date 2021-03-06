#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        long long vm_bytes;
        long long resident_bytes;
        long long vm_bytes_peak;
        long long resident_bytes_peak;
    } memory_stats_t;


    extern
    int get_self_memory_usage(memory_stats_t * out_stats);
    
    extern
    int get_process_memory_usage(int pid, memory_stats_t * out_stats);

    extern
    int get_self_vm_bytes(long long * out_bytes);

    extern
    int get_self_vm_bytes_peak(long long * out_bytes);

    extern
    int get_self_resident_bytes(long long * out_bytes);

    extern
    int get_self_resident_bytes_peak(long long * out_bytes);

#ifdef __cplusplus
}
#endif

