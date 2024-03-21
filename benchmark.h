#ifndef INCLUDE_BENCHMARK_H

#define INCLUDE_BENCHMARK_H
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

struct timespec timer_start();
uint64_t timer_end(struct timespec start_time);
uint64_t test_func(uint64_t (*test_func)(), uint64_t const times);
uint64_t test_func_total(void (*test_func)(), uint64_t const times);

#endif
