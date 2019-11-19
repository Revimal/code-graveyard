#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "fastcpy-avx.h"

#define BENCHMARK_DATA_SIZE 2048
#define BENCHMARK_DATA_CYCLE 256

#define BENCHMARK_BARRIER_MEMORY() \
	__asm__ __volatile__ ("lock; addl $0,-4(%%rsp)" ::: "memory", "cc")

#define BENCHMARK_BARRIER_OPTIMIZATION() \
	__asm__ __volatile__("": : :"memory")

static uint8_t orig_arr[BENCHMARK_DATA_SIZE];
static uint8_t norm_arr[BENCHMARK_DATA_SIZE];
static uint8_t fast_arr[BENCHMARK_DATA_SIZE];

static inline unsigned long rdtsc_now(void)
{
	register unsigned int l32 __asm__("eax");
	register unsigned int h32 __asm__("edx");
	__asm__ __volatile__ (
			"rdtsc"
			: "=a" (l32), "=d" (h32)
			:: "memory");
	return ((unsigned long)h32 << 32) | ((unsigned long)l32);
}

int main(int argc, char *argv[])
{
	int i;
	uint64_t norm_begin, norm_end;
	uint64_t fast_begin, fast_end;

	for (i = 0; i < BENCHMARK_DATA_SIZE; ++i)
	{
		orig_arr[i] = i % BENCHMARK_DATA_CYCLE;
	}

	BENCHMARK_BARRIER_MEMORY();

	norm_begin = rdtsc_now();
	BENCHMARK_BARRIER_OPTIMIZATION();
	memcpy(norm_arr, orig_arr, BENCHMARK_DATA_SIZE);
	BENCHMARK_BARRIER_OPTIMIZATION();
	norm_end = rdtsc_now();

	BENCHMARK_BARRIER_MEMORY();

	fast_begin = rdtsc_now();
	BENCHMARK_BARRIER_OPTIMIZATION();
	fastcpy256(fast_arr, orig_arr);
	fastcpy256(fast_arr+256, orig_arr+256);
	fastcpy256(fast_arr+512, orig_arr+512);
	fastcpy256(fast_arr+768, orig_arr+768);
	fastcpy256(fast_arr+1024, orig_arr+1024);
	fastcpy256(fast_arr+1280, orig_arr+1280);
	fastcpy256(fast_arr+1536, orig_arr+1536);
	fastcpy256(fast_arr+1792, orig_arr+1792);
	BENCHMARK_BARRIER_OPTIMIZATION();
	fast_end = rdtsc_now();

	BENCHMARK_BARRIER_MEMORY();

	printf("[NORMCPY]: BASE=%020"PRIu64", PROCESS=%020"PRIu64", DELAY=%020"PRIu64"\n",
			norm_begin, norm_end, (norm_end-norm_begin));
	printf("[FASTCPY]: BASE=%020"PRIu64", PROCESS=%020"PRIu64", DELAY=%020"PRIu64"\n",
			fast_begin, fast_end, (fast_end-fast_begin));

	printf("\n");

	printf("=== normcpy256 ===\n");
	for (i = 0; i < BENCHMARK_DATA_SIZE; ++i)
		printf("%03d %s", norm_arr[i], !((i + 1) % 32) ? "\n" : "");

	printf("=== fastcpy256 ===\n");
	for (i = 0; i < BENCHMARK_DATA_SIZE; ++i)
		printf("%03d %s", fast_arr[i], !((i + 1) % 32) ? "\n" : "");

	return 0;
}
