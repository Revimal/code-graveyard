#ifndef X86_COROUTINE_H
#define X86_COROUTINE_H

/* INTERNAL: common cache line size of most modern processors. */
#define X86_COROUTINE_CACHESZ 64

/* INTERNAL: macro for aligning memory to the cache line size. */
#define X86_COROUTINE_ALIGNED \
	__attribute__((__aligned__(X86_COROUTINE_CACHESZ)))

#ifdef __x86_64__
#ifdef __AVX__
#define X86_COROUTINE_CTX_SIMD_CLOBBER \
	, "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7",  \
	"ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13", "ymm14", "ymm15"
#else /* __AVX__ */
#define X86_COROUTINE_CTX_SIMD_CLOBBER \
	, "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",  \
	"xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"
#endif /* __AVX__ */

/**
 * @brief A structure stores registers of a physical core.
 *
 * The amd64(a.k.a x86_64) architecture has several kinds of registers.
 *
 * Stack registers:
 *	- RSP		: descirbe next push/pop location in current stack frame.
 *	- RBP		: describe start address of current stack frame.
 * Arithimetic registers:
 *	- RAX		: be used as a return value in most ABIs.
 *	- RBX		: be used as a general purpose register in most ABIs.
 *	- RCX		: be used as an above, but especially for loop iterating.
 *	- RDX		: be used as a general purpose register at most ABIS.
 * Index operation registers:
 *	- RSI		: be used as a string copy/move source pointer in most ABIs.
 *	- RDI		: be used as a string copy/move destination pointer in most ABIs.
 * General purpose registers:
 *	- R8:15
 * x87 FPU registers (a.k.a MMX registers):
 *	- (deprecated, replaced by SSE/AVX registers in most modern compilers)
 * 128-bit SIMD Registers (a.k.a SSE/AVX):
 *	- XMM0:15	: be used for the 128-bit vectorized data processing.
 * 256-bit SIMD Registers (a.k.a AVX2):
 *	- YMM0:15	: be used for the 256-bit vectorized data preocessing.
 * 512-bit SIMD Registers (only for XEON-PHI):
 *	- ZMM0:15	: be used for the 512-bit vectorized data processing.
 *
 * 1)
 * The System-V ABI, that Linux uses, stores following registers in a stack
 * when compiler determines they could be tainted during calling functions.
 *  - RAX: for a function return.
 *  - RDI, RSI, RDX, RCX, R8, R9, XMM0-7: for function parameters.
 *
 * 2)
 * Mostly, SIMD registers are used as temporary operands.
 * So, they can be changed freely when vectorized instructions have ended.
 *
 * According to the above-mentioned descriptions,
 * we only need to stores following registers in a stack
 * to implement light-weight context.
 */
struct x86_coroutine_ctx
{
	void *rsp; /**< Stack pointer register.		(offset: 0 ) */
	void *rbp; /**< Stack frame base register.	(offset: 8 ) */
	void *rip; /**< Program counter register.	(offset: 16) */
	void *rbx; /**<	Arithmetic register.		(offset: 24) */
	void *r12; /**< General purpose register.	(offset: 32) */
	void *r13; /**< General purpose register.	(offset: 40) */
	void *r14; /**< General purpose register.	(offset: 48) */
	void *r15; /**< General purpose register.	(offset: 56) */
} X86_COROUTINE_ALIGNED;

extern void x86_coroutine_save_archctx(struct x86_coroutine_ctx *ctx);
extern void x86_coroutine_restore_archctx(struct x86_coroutine_ctx *ctx);
extern void x86_coroutine_switch_archctx(struct x86_coroutine_ctx *curr_ctx, struct x86_coroutine_ctx *saved_ctx);

/** Store current context into the ctx structure @p ctx. */
#define x86_coroutine_save_ctx( ctx ) \
	__asm__ __volatile__ ( \
			"movq %0, %%rdi\n\t" \
			"call %P1\n\t" \
			: : "r"(ctx), "g"(x86_coroutine_save_archctx) \
			: "memory", "cc", "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11" \
			X86_COROUTINE_CTX_SIMD_CLOBBER \
			)

/** Load the saved context to machine registers. */
#define x86_coroutine_restore_ctx( ctx ) \
	__asm__ __volatile__ ( \
			"movq %0, %%rdi\n\t" \
			"call %P1\n\t" \
			: : "r"(ctx), "g"(x86_coroutine_restore_archctx) \
			: "memory", "cc", "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11" \
			X86_COROUTINE_CTX_SIMD_CLOBBER \
			)

/** Store current context into @p curr_ctx, then load @p saved_ctx. */
#define x86_coroutine_switch_ctx( curr_ctx, saved_ctx ) \
	__asm__ __volatile__ ( \
			"movq %0, %%rdi\n\t" \
			"movq %1, %%rsi\n\t" \
			"call %P2\n\t" \
			: : "r"(curr_ctx), "r"(saved_ctx), "g"(x86_coroutine_switch_archctx) \
			: "memory", "cc", "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11" \
			X86_COROUTINE_CTX_SIMD_CLOBBER \
			)

#else /* __x86_64__ */
#pragma GCC error "unsupported environment"

#endif /* __x86_64__ */

#endif /* X86_COROUTINE_H */
