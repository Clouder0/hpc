#include <stdio.h>
#include <stdint.h>
#include "../benchmark.h"

void random_fill(size_t n, size_t m, double arr[n][m]) {
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < m; ++j)
            arr[i][j] = rand();  // 32bit integer random number
}

#define N 512
#define M 512
#define K 512
#define TIMES 20
#define SZ 16

double A[N][M], B[M][K], C[N][K];
void test_ikj() {
    for (register size_t i = 0; i < N; ++i)
        for (register size_t k = 0; k < K; ++k)
        {
            register double t = A[i][k];                
            for (register size_t j = 0; j < M; ++j)
                C[i][j] += t * B[k][j];
        }
}

void test_ijk() {
    for (register size_t i = 0; i < N; ++i)
        for (register size_t j = 0; j < M; ++j)
        {
            register double t = 0;                
            for (register size_t k = 0; k < K; ++k)
                t += A[i][k] * B[k][j];
            C[i][j] = t;
        }
}
void test_block() {
    for (register size_t i = 0; i < N; i += SZ)
        for (register size_t j = 0; j < M; j += SZ)
        {
            for (register size_t ii = i; ii < i + SZ; ++ii)
            {
                // A[i:i+SZ][0:K] * B[0:K][j:j+SZ]
                for (register size_t k = 0; k < K; ++k)
                {
                    register double t = A[ii][k];
                    for (register size_t jj = j; jj < j + SZ; ++jj)
                        C[ii][jj] += t * B[k][jj];
                }
            }
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
        test_block();
        test_ijk();
        test_ikj();
    }
    printf("warming up takes %lld ms.\n", timer_end(start) / 1000000);
    uint64_t sum_block = test_func_total(test_block, TIMES);
    uint64_t sumijk = test_func_total(test_ijk, TIMES);
    uint64_t sumikj = test_func_total(test_ikj, TIMES);
    printf("total time of block: %lld ms, %.2f GFLOPS\n", sum_block / 1000000, calc_gflops(sum_block));
    printf("total time of ijk: %lld ms, %.2f GFLOPS\n", sumijk / 1000000, calc_gflops(sumijk));
    printf("total time of ikj: %lld ms, %.2f GFLOPS\n", sumikj / 1000000, calc_gflops(sumikj));
    return 0;
}
