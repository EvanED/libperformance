#include <iostream>

#include "getmemusage.h"

////////////////////////////////////////////
// I should put this in its own header....
////////////////////////////////////////////

#define ASSERT_EQUAL(a, b) AssertEqual((a), (b), __FILE__, __LINE__, #a, #b)
#define ASSERT_LTE(a, b) AssertLessEqual((a), (b), __FILE__, __LINE__, #a, #b)
#define ASSERT_CLOSE(a, b) AssertClose((a), (b), __FILE__, __LINE__, #a, #b)

template<typename T>
void
AssertClose(T const & a, T const & b,
            char const * file, int line,
            char const * expr_a, char const * expr_b)
{
    T diff = a - b;
    double pct = static_cast<double>(diff) / a;
    if (pct > 0.01) {
        std::cout << "Assertion failed: " << expr_a << " == " << expr_b << ":\n"
                  << "    " << expr_a << " is " << a << "\n"
                  << "    " << expr_b << " is " << b << "\n"
                  << "    percentage difference: " << pct*100 << "\n";
    }
}

template<typename T, typename U>
void
AssertEqual(T const & a, U const & b,
            char const * file, int line,
            char const * expr_a, char const * expr_b)
{
    if (a != b) {
        std::cout << "Assertion failed: " << expr_a << " == " << expr_b << ":\n"
                  << "    " << expr_a << " is " << a << "\n"
                  << "    " << expr_b << " is " << b << "\n";
    }
}

template<typename T, typename U>
void
AssertLessEqual(T const & a, U const & b,
                char const * file, int line,
                char const * expr_a, char const * expr_b)
{
    if (a > b) {
        std::cout << "Assertion failed: " << expr_a << " <= " << expr_b << ":\n"
                  << "    " << expr_a << " is " << a << "\n"
                  << "    " << expr_b << " is " << b << "\n";
    }
}

///////////////////
// Now we can go.
///////////////////

const long long five_hundred_mb = 1024 * 1024 * 500;

int main(int argc, char** argv)
{
    bool good = true;
    
    memory_stats_t before, between, after;
    
    int ret = get_self_memory_usage(&before);
    if (ret) {
        perror(argv[0]);
        exit(1);
    }

    char* c = new char[five_hundred_mb]();

    ret = get_self_memory_usage(&between);
    if (ret) {
        perror(argv[0]);
        exit(1);
    }

    delete[] c;

    ret = get_self_memory_usage(&after);
    if (ret) {
        perror(argv[0]);
        exit(1);
    }


    // The memory usage should have increased from before->between:
    ASSERT_LTE(before.vm_bytes, between.vm_bytes);
    ASSERT_LTE(before.vm_bytes_peak, between.vm_bytes_peak);
    ASSERT_LTE(before.resident_bytes, between.resident_bytes);
    ASSERT_LTE(before.resident_bytes_peak, between.resident_bytes_peak);
    
    long long
        vm_increase_before_to_between = between.vm_bytes - before.vm_bytes,
        rss_increase_before_to_between = between.resident_bytes - before.resident_bytes,
        vm_peak_increase_before_to_between = between.vm_bytes_peak - before.vm_bytes_peak,
        rss_peak_increase_before_to_between = between.resident_bytes_peak - before.resident_bytes_peak;

    ASSERT_CLOSE(vm_increase_before_to_between, five_hundred_mb);
    ASSERT_CLOSE(rss_increase_before_to_between, five_hundred_mb);
    ASSERT_CLOSE(vm_peak_increase_before_to_between, five_hundred_mb);
    ASSERT_CLOSE(rss_peak_increase_before_to_between, five_hundred_mb);

    // The memory usage may have dropped from between->after, but should not have risen:
    ASSERT_LTE(after.vm_bytes, between.vm_bytes);
    ASSERT_LTE(after.resident_bytes, between.resident_bytes);

    long long
        vm_decrease_between_to_after = between.vm_bytes - after.vm_bytes,
        rss_decrease_between_to_after = between.resident_bytes - after.resident_bytes;

    ASSERT_CLOSE(vm_decrease_between_to_after, five_hundred_mb);
    ASSERT_CLOSE(rss_decrease_between_to_after, five_hundred_mb);

    // But the peaks should not have changed.
    ASSERT_EQUAL(after.vm_bytes_peak, between.vm_bytes_peak);
    ASSERT_EQUAL(after.resident_bytes_peak, between.resident_bytes_peak);
}
