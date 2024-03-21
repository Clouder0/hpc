benchmark.o: benchmark.c
	cc -c benchmark.c -o benchmark.o -march=native -O3
	
# match all gemm_*.o
gemm_%.o: gemm/%.c benchmark.o
	cc -c gemm/$*.c -o gemm/$*.o -march=native -O3

# match all gemm_*
gemm_%: gemm_%.o benchmark.o
	cc gemm/$*.o benchmark.o -o gemm/$*.out -march=native -O3

CLEAN: 
	rm -f *.o
	rm -f gemm/*.o
	rm -f gemm/*.out
