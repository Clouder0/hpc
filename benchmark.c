#include "benchmark.h"

struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

uint64_t timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    uint64_t diffInNanos = (end_time.tv_sec - start_time.tv_sec) * 1000000000ull + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

uint64_t test_func(uint64_t (*test_func)(), uint64_t const times) {
    register uint64_t res = 0;
    for (register uint64_t i = 0; i < times; ++i)
        res += test_func();
    return res;
}

uint64_t test_func_total(void (*test_func)(), uint64_t const times)
{
    struct timespec start = timer_start();
    for (uint64_t i = 0; i < times; ++i)
        test_func();
    return timer_end(start);
}