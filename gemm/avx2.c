#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "../benchmark.h"

void random_fill(size_t n, size_t m, float arr[n][m]) {
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < m; ++j)
            arr[i][j] = rand();  // 32bit integer random number
}

#define N 512
#define M 512
#define K 512
#define TIMES 100
#define SZ 4


float A[N][M], B[M][K], C[N][K], D[K][M];
void test_ikj() {
    for (register size_t i = 0; i < N; ++i)
        for (register size_t k = 0; k < K; ++k)
        {
            register float t = A[i][k];                
            for (register size_t j = 0; j < M; ++j)
                C[i][j] += t * B[k][j];
        }
}

void test_ijk() {
    for (register size_t i = 0; i < N; ++i)
        for (register size_t j = 0; j < M; ++j)
        {
            register float t = 0;                
            for (register size_t k = 0; k < K; ++k)
                t += A[i][k] * B[k][j];
            C[i][j] = t;
        }
}

void handle_block(size_t i, size_t j)
{
    float res[4];
    for (register size_t ii = i; ii < i + SZ; ++ii)
    {
        // result vector
        for (register size_t jj = j; jj < j + SZ; ++jj)
        {
            __m256 r = _mm256_setzero_ps();
            for (register size_t k = 0; k < N; k += 32)
            {
                __m256 a0 = _mm256_loadu_ps(A[ii] + k);
                __m256 a1 = _mm256_loadu_ps(A[ii] + k + 8);
                __m256 a2 = _mm256_loadu_ps(A[ii] + k + 16);
                __m256 a3 = _mm256_loadu_ps(A[ii] + k + 24);
                __m256 b0 = _mm256_loadu_ps(D[jj] + k);
                __m256 b1 = _mm256_loadu_ps(D[jj] + k + 8);
                __m256 b2 = _mm256_loadu_ps(D[jj] + k + 16);
                __m256 b3 = _mm256_loadu_ps(D[jj] + k + 24);
                __m256 r0 = _mm256_add_ps(_mm256_mul_ps(a1, b1), _mm256_mul_ps(a0, b0));
                __m256 r1 = _mm256_add_ps(_mm256_mul_ps(a3, b3), _mm256_mul_ps(a2, b2));
                r = _mm256_add_ps(r, _mm256_add_ps(r0, r1));
            }
            _mm256_storeu_ps(res, r);
            C[ii][jj] = res[0] + res[1] + res[2] + res[3];
        }
    }
}
void test_block()
{
    // transpose first
    for (register size_t i = 0; i < M; ++i)
        for (register size_t j = 0; j < K; ++j)
            D[j][i] = B[i][j];
    for (register size_t i = 0; i < N; i += SZ)
        for (register size_t j = 0; j < M; j += SZ)
            handle_block(i, j);
}

float calc_gflops(uint64_t time_ns) {
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
    printf("%.2f\n", C[2][3]);
    uint64_t sumijk = test_func_total(test_ijk, TIMES);
    printf("%.2f\n", C[2][3]);
    uint64_t sumikj = test_func_total(test_ikj, TIMES);
    printf("%.2f\n", C[2][3]);
    printf("total time of block: %lld ms, %.2f GFLOPS\n", sum_block / 1000000, calc_gflops(sum_block));
    printf("total time of ijk: %lld ms, %.2f GFLOPS\n", sumijk / 1000000, calc_gflops(sumijk));
    printf("total time of ikj: %lld ms, %.2f GFLOPS\n", sumikj / 1000000, calc_gflops(sumikj));
    return 0;
}
