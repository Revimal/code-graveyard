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

* **[GARP Sender](https://github.com/Revimal/code-graveyard/blob/master/garp-sender)** (Language: C++11)\
	The standalone toolkit to send GARP packet for testing HA failover actions. \
    It was made with the **[libtins](http://libtins.github.io/)** library to simplify the implementation of packet manipulations. \
    You can build this toolkit with 'g\+\+ -o garp-sender garp-sender.cpp -std=c\+\+11 -ltins'. \
    As a matter of course, you must install the **[libtins](http://libtins.github.io/)** before you compile it.

* **[Portable RMSpace](https://github.com/Revimal/code-graveyard/blob/master/portable-rmspace)** (Language: C)\
	This single source file is made to remove whitespaces(TAB, SPACE, NEW-LINE, CARRIAGE-RETURN) as efficiently as in any environment. \
    It is implemented by a standard-compatible bit-twiddling hack, not any architecture-depend or compiler-depend features. \
    This one removes whitespaces typically about 1.5 to 2 times faster than the traditional for-loop-based algorithm. \
    If you know the most optimized algorithm for the target architecture (such as AVX512VBMI), use your one instead of this catastrophically incomprehensible code. \
    You can check the benchmark result produced on my machine and its source code in path '[portable-rmspace/benchmark](https://github.com/Revimal/code-graveyard/blob/master/portable-rmspace/benchmark)'. \
    It shows a 65% reduction in performance impact.

* **[GDB Loopchk](https://github.com/Revimal/code-graveyard/blob/master/gdb-loopchk)** (Language: GDB-Python)\
	'GDB Loopchk' is a GDB plugin for detect a linked list has a loop.
    ```c
    struct list_entry {
        struct list_entry *next;
    };

    int main(int argc, char *argv[])
    {
        struct list_entry a, b, c;

        a.next = &b;
        b.next = &c;
        c.next = &a;

        return 0;
    }
    ```
    ```
    (gdb) p a
    $1 = {next = 0x7fffffffe338}
    (gdb) p b
    $2 = {next = 0x7fffffffe340}
    (gdb) p c
    $3 = {next = 0x7fffffffe330}
    (gdb) source loopchk.py
    (gdb) loopchk 10 &a
    Found looping with 2 nodes:
    0: 0x7fffffffe330
    1: 0x7fffffffe338
    2: 0x7fffffffe340
    ```
    If you need more information, read this [manual](https://github.com/Revimal/code-graveyard/blob/master/gdb-loopchk/manual.md)!
