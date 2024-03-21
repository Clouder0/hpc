#include <stdio.h>
#include <immintrin.h>

double a[32][32], b[32][32], c[32][32], d[32][32], e[32][32];

void baseline()
{
    for (register size_t i = 0; i < 32; ++i)
        for (register size_t k = 0; k < 32; ++k)
        {
            register double t = a[i][k];                
            for (register size_t j = 0; j < 32; ++j)
                d[i][j] += t * b[k][j];
        }
}

void handle_block(size_t i, size_t j) {
    double res[4];
    for(register size_t ii = i; ii < i + 16; ++ii)
    {
        // result vector
        for (register size_t jj = j; jj < j + 16; ++jj)
        {
            __m256d r = _mm256_setzero_pd();
            for (register size_t k = 0; k < 32; k += 16)
            {
                __m256d a0 = _mm256_loadu_pd(a[ii] + k);
                __m256d a1 = _mm256_loadu_pd(a[ii] + k + 4);
                __m256d a2 = _mm256_loadu_pd(a[ii] + k + 8);
                __m256d a3 = _mm256_loadu_pd(a[ii] + k + 12);
                __m256d b0 = _mm256_loadu_pd(e[jj] + k);
                __m256d b1 = _mm256_loadu_pd(e[jj] + k + 4);
                __m256d b2 = _mm256_loadu_pd(e[jj] + k + 8);
                __m256d b3 = _mm256_loadu_pd(e[jj] + k + 12);
                __m256d r0 = _mm256_add_pd(_mm256_mul_pd(a1, b1), _mm256_mul_pd(a0, b0));
                __m256d r1 = _mm256_add_pd(_mm256_mul_pd(a3, b3), _mm256_mul_pd(a2, b2));
                r = _mm256_add_pd(r, _mm256_add_pd(r0, r1));
            }
            _mm256_storeu_pd(res, r);
            c[ii][jj] = res[0] + res[1] + res[2] + res[3];
            printf("c[%d][%d] = %lf\n", ii, jj, c[ii][jj]);
        }
    }
}

void tranpose() {
    for(int i = 0; i < 32;++i) for(int j = 0; j < 32;++j) e[i][j] = b[j][i];
}
int main() {
    for(size_t i = 0; i < 32;++i) for(size_t j = 0; j < 32;++j) a[i][j] = i + j;
    for(size_t i = 0; i < 32;++i) for(size_t j = 0; j < 32;++j) b[i][j] = i * j;
    // print A and B
    for (int i = 0; i < 32; puts(""), ++i)
        for (int j = 0; j < 32; ++j)
            printf("%lf ", a[i][j]);
    tranpose();
    handle_block(0, 0);
    handle_block(0, 16);
    handle_block(16, 0);
    handle_block(16, 16);
    baseline();
    for (int i = 0; i < 32; puts(""), ++i)
        for (int j = 0; j < 32; ++j)
            if(d[i][j] != c[i][j]) printf("d[%d][%d] = %lf, c[%d][%d] = %lf\n", i, j, d[i][j], i, j, c[i][j]);
    return 0;
}