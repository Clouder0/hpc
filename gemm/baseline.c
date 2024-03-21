#include <stdio.h>
#include <stdint.h>
#include "../benchmark.h"

void random_fill(size_t n, size_t m, uint64_t arr[n][m]) {
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < m; ++j)
            arr[i][j] = rand();  // 32bit integer random number
}

#define N 512
#define M 512
#define K 512
#define TIMES 20

uint64_t A[N][M], B[M][K], C[N][K];
void test_ikj() {
    for (register size_t i = 0; i < N; ++i)
        for (register size_t k = 0; k < K; ++k)
        {
            register uint64_t t = A[i][k];                
            for (register size_t j = 0; j < M; ++j)
                C[i][j] += t * B[k][j];
        }
}

void test_ijk() {
    for (register size_t i = 0; i < N; ++i)
        for (register size_t j = 0; j < M; ++j)
        {
            register uint64_t t = 0;                
            for (register size_t k = 0; k < K; ++k)
                t += A[i][k] * B[k][j];
            C[i][j] = t;
        }
}

double calc_gflops(uint64_t time_ns) {
    return 2.0 * N * M * K * TIMES / time_ns;
}

int main(void) {
    srand(114514);
    random_fill(N, M, A);
    random_fill(M, K, B);

    // warming up
    struct timespec start = timer_start();
    for(int i = 0; i < 10; ++i) {
        test_ikj();
        test_ijk();
    }
    printf("warming up takes %lld ms.\n", timer_end(start) / 1000000);
    uint64_t sumikj = test_func_total(test_ikj, TIMES);
    uint64_t sumijk = test_func_total(test_ijk, TIMES);
    printf("total time of ikj: %lld ms, %.2f GFLOPS\n", sumikj / 1000000, calc_gflops(sumikj));
    printf("total time of ijk: %lld ms, %.2f GFLOPS\n", sumijk / 1000000, calc_gflops(sumijk));
    // printf("total time of 2: %lld ms\n", sum2 / 1000000);
    return 0;
}
