# code-graveyard
The graveyard for code-snippets that abandoned from my projects due to the over-engineering.

I want to ignore any kind of issue about these deprecated codes,
  but you can report it on '[Github Issue Tracker](https://github.com/Revimal/code-graveyard/issues)' if you have problems using them,
  nonetheless.

All contents in this repository are licensed under a [Beerware License](https://github.com/Revimal/code-graveyard/blob/master/LICENSE).

## CONTENTS
* **[x86 Coroutine](https://github.com/Revimal/code-graveyard/blob/master/x86-coroutine)** (Language: C, ASM-GCC)\
	Technically, this is an implementation of 'amd64' coroutine, not for 'pitty-old-32b-i386'.\
	Is there anybody compalaining about this? Nevermind :P\
	Supports clobbering for AVX/AVX2 SIMD registers.

* **[SIGSEGV mmap](https://github.com/Revimal/code-graveyard/blob/master/sigsegv-mmap)** (Language: C)\
	This snippet contains subroutines that try memory-mapping for an address, which caused the segmentation fault.\
	Only tested on 'x86_64 linux 4.15', so if the prototype of 'sigaction' handler is different than my environment, this may not work.\
	It uses 'Anonymous memory mapping', but you can change it in any form. (e.g. do physical memory mapping from /dev/mem)

* **[Fastcpy AVX](https://github.com/Revimal/code-graveyard/blob/master/fastcpy-avx)** (Language: C, ASM-GCC)\
	The implementation of super-fast memcpy() for the 256-bit aligned memory address.\
	It requires an 'amd64' machine which supports AVX's 'VMOVUPS' instruction. \
	You can test this snippet with 'gcc -o benchmark benchmark.c; ./benchmark' commands. \
	Well... you don't have any GCC build system which lied on Linux? THATS A SHAME :M

* **[Handler Template](https://github.com/Revimal/code-graveyard/blob/master/handler-template)** (Language: C++17)\
	The beautiful codeset for programmers who badly addicted to the template-meta-programming. \
	'Whoever does not use the SFINAE has no heart. Whoever want it use has no time to rest.' \
	Q: Why didn't use 'std::bind'? It makes codes more clarifying. \
	A: Well... just-for-fun...?
