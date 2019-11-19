#if defined (__x86_64__)

	# save lctx
	.text
	.p2align 4,,15
	.globl x86_couroutine_save_archctx
	.type x86_couroutine_save_archctx, @function
x86_couroutine_save_archctx:
	movq %rsp, 0(%rdi)	#save stack pointer
	movq %rbp, 8(%rdi)	#save frame pointer
	movq (%rsp), %rax	#get instruction pointer (call-stack)
	movq %rax, 16(%rdi)	#save instruction pointer
	movq %rbx, 24(%rdi)	#save rbx register
	movq %r12, 32(%rdi)	#save r12 register
	movq %r13, 40(%rdi)	#save r13 register
	movq %r14, 48(%rdi)	#save r14 register
	movq %r15, 56(%rdi)	#save r15 register
	xorq %rax, %rax		#clean rax register to zero
	ret
	.size x86_couroutine_save_archctx,.-x86_couroutine_save_archctx

	# restore lctx
	.text
	.p2align 4,,15
	.globl x86_couroutine_restore_archctx
	.type x86_couroutine_restore_archctx, @function
x86_couroutine_restore_archctx:
	movq 24(%rdi), %rbx	#restore rbx register
	movq 32(%rdi), %r12	#restore r12 register
	movq 40(%rdi), %r13	#restore r13 register
	movq 48(%rdi), %r14	#restore r14 register
	movq 56(%rdi), %r15 #restore r15 register
	movq 0(%rdi), %rsp	#restore stack pointer
	movq 8(%rdi), %rbp	#restore frame pointer
	movq 16(%rdi), %rax #get instruction pointer
	movq %rax, (%rsp)	#restore instruction pointer
	xorq %rax, %rax		#clean rax register to zero
	ret
	.size x86_couroutine_restore_archctx,.-x86_couroutine_restore_archctx

	# direct-switch lctx
	.text
	.p2align 4,,15
	.globl x86_couroutine_switch_archctx
	.type x86_couroutine_switch_archctx, @function
x86_couroutine_switch_archctx:
	movq %rsp, 0(%rdi)	#save curr stack pointer
	movq %rbp, 8(%rdi)	#save curr frame pointer
	movq (%rsp), %rax	#get curr instruction pointer (call-stack)
	movq %rax, 16(%rdi)	#save curr instruction pointer
	movq %rbx, 24(%rdi)	#save curr rbx register
	movq %r12, 32(%rdi)	#save curr r12 register
	movq %r13, 40(%rdi)	#save curr r13 register
	movq %r14, 48(%rdi)	#save curr r14 register
	movq %r15, 56(%rdi)	#save curr r15 register
	movq 24(%rsi), %rbx	#restore saved rbx register
	movq 32(%rsi), %r12	#restore saved r12 register
	movq 40(%rsi), %r13	#restore saved r13 register
	movq 48(%rsi), %r14	#restore saved r14 register
	movq 56(%rsi), %r15 #restore saved r15 register
	movq 0(%rsi), %rsp	#restore saved stack pointer
	movq 8(%rsi), %rbp	#restore frame pointer
	movq 16(%rsi), %rax #get saved instruction pointer
	movq %rax, (%rsp)	#restore saved instruction pointer
	movq %rax, %rax		#clean rax register to zero
	ret
	.size x86_couroutine_switch_archctx,.-x86_couroutine_switch_archctx

#else
#pragma GCC error "unsupported environment"
#endif
