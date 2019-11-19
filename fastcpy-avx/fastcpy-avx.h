#ifndef FASTCPY_AVX_H
#define FASTCPY_AVX_H

static inline void fastcpy256(unsigned char *dest, const unsigned char *src) {
	__asm__ __volatile__ (
			"vmovups 0(%0), %%ymm0\n\t"
			"vmovups 32(%0), %%ymm1\n\t"
			"vmovups %%ymm0, 0(%1)\n\t"
			"vmovups %%ymm1, 32(%1)\n\t"
			"vmovups 64(%0), %%ymm0\n\t"
			"vmovups 96(%0), %%ymm1\n\t"
			"vmovups %%ymm0, 64(%1)\n\t"
			"vmovups %%ymm1, 96(%1)\n\t"
			"vmovups 128(%0), %%ymm0\n\t"
			"vmovups 160(%0), %%ymm1\n\t"
			"vmovups %%ymm0, 128(%1)\n\t"
			"vmovups %%ymm1, 160(%1)\n\t"
			"vmovups 192(%0), %%ymm0\n\t"
			"vmovups 224(%0), %%ymm1\n\t"
			"vmovups %%ymm0, 192(%1)\n\t"
			"vmovups %%ymm1, 224(%1)\n\t"
			: : "r" (src), "r" (dest) : "memory");
}

#endif /* FASTCPY_AVX_H */
