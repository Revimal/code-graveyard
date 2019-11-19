#ifndef SIGSEGV_MMAP_H
#define SIGSEGV_MMAP_H

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

/* Can use 'sysconf(_SC_PAGESIZE)' for more accurate page-alignment,
 * but 'sysconf()' is not a reentrant function so I didn't. */
#define SIGSEGV_MMAP_PAGESZ 4096

static inline void sigsegv_mmap_handler( int sig, siginfo_t *siginf, void *ctx )
{
	if ( !siginf || siginf->si_signo != SIGSEGV )
	{
		goto failed;
	}

	if ( mmap( siginf->si_addr, SIGSEGV_MMAP_PAGESZ, PROT_READ | PROT_WRITE,
				MAP_ANONYMOUS | MAP_PRIVATE, -1, 0 ) == MAP_FAILED )
	{
		goto failed;
	}

	return;

failed:
	signal( siginf->si_signo, SIG_DFL );
	return;
}

static inline int sigsegv_mmap_attach( void )
{
	struct sigaction sigact;

	sigact.sa_sigaction = sigsegv_mmap_handler;
	sigact.sa_flags = SA_SIGINFO;

	if ( sigaction( SIGSEGV, &sigact, NULL ) < 0 )
	{
		return errno;
	}

	return 0;
}

static inline void sigsegv_mmap_detach( void )
{
	signal( SIGSEGV, SIG_DFL );
}

#endif /* SIGSEGV_MMAP_H */
