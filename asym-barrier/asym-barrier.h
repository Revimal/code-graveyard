/**
 * @file asym_barrier.h
 * @brief The 'asymmetric-barrier' implementations.
 *
 * The 'asymmetric-barrier' is a synchronous method
 * that is consisted of a single updater and multiple waiters.
 *
 * The updater can update the period of a barrier.
 *
 * After the period has updated, a waiter that reached
 * its barrier point must wait for others to reach their one.
 *
 * Simultaneously, the updater can do useful works
 * to prepare for the next period.
 *
 * If all useful works, described at the above, has done,
 * the updater is considered as reaching the barrier point.
 *
 * After all of the workers have reached its barrier point,
 * the updater can expire all barrier points, including itself's.
 * (The word 'workers' is a collective term for the updater and waiters.)
 *
 * Now, the asymmetric-barries has fully synced.
 *
 * Once an asymmetric-barrier has synced,
 * all waiters will not block until the updater requesting the new period.
 *
 * @author Hyeonho Seo a.k.a Revimal (seohho@gmail.com)
 */

#ifndef ASYM_BARRIER_H
#define ASYM_BARRIER_H

#ifndef __KERNEL__
#include <stdint.h>
#else /* __KERNEL__ */
#include <linux/types.h>
#endif /* __KERNEL__ */

#define ASYM_BARRIER_LIKELY(x) \
	__builtin_expect(!!(x), 1)
#define ASYM_BARRIER_UNLIKELY(x) \
	__builtin_expect(!!(x), 0)
#define ASYM_BARRIER_RELAX() \
	__asm__ __volatile__ ("pause")
#define ASYM_BARRIER_CACHE_SIZE 64
#define ASYM_BARRIER_CACHE_ALIGNED \
	__attribute__((aligned(ASYM_BARRIER_CACHE_SIZE)))

/** @brief A structure describing the asymmetric barrier object. */
struct asym_barrier_obj
{
	uint64_t refcnt;	/**< Number of waiters referencing this. */
	uint64_t wcount;	/**< Number of waiters currently waiting on its barrier point. */
	uint64_t synced;	/**< Synchronization counter (Internal use only). */
} ASYM_BARRIER_CACHE_ALIGNED;

/** @brief Typedef for the asymmetric barrier object. */
typedef struct asym_barrier_obj ASYM_BARRIER_CACHE_ALIGNED asym_barrier_t;

/**
 * @brief Initialize an asymmetric barrier object.
 *
 * @param asymb
 *	The pointer to an asymmetric barrier object.
 * @param waiters
 *	The number of waiters for the object.
 */
static inline void asym_barrier_init(asym_barrier_t *asymb, uint64_t waiters)
{
	if (ASYM_BARRIER_UNLIKELY(!asymb))
		return;

	__atomic_store_n(&asymb->refcnt, waiters, __ATOMIC_RELAXED);
	__atomic_store_n(&asymb->wcount, 0, __ATOMIC_RELAXED);
	__atomic_store_n(&asymb->synced, 0, __ATOMIC_RELAXED);

	return;
}

/**
 * @brief Update the period of an asymmetric barrier object.
 *
 * This function cancels workers' synchronized states.
 *
 * @param asymb
 *	The pointer to an asymmetric barrier object.
 * @param synced
 *	The flag to make the updater waiting for all waiters to reach its barrier point.
 *	If 0, this function will not wait for waiters.
 */
static inline void asym_barrier_update(asym_barrier_t *asymb, unsigned synced)
{
	if (ASYM_BARRIER_UNLIKELY(!asymb))
		return;

	__atomic_store_n(&asymb->wcount, asymb->refcnt, __ATOMIC_SEQ_CST);

	while (synced && __atomic_load_n(&asymb->wcount, __ATOMIC_RELAXED))
		ASYM_BARRIER_RELAX();

	return;
}

/**
 * @brief Commit the updated period to an asymmetric barrier object.
 *
 * This function waits for all workers are fully synced to the updated period.
 *
 * @param asymb
 *	The pointer to an asymmetric barrier object.
 */
static inline void asym_barrier_commit(asym_barrier_t *asymb)
{
	if (ASYM_BARRIER_UNLIKELY(!asymb))
		return;

	while (__atomic_load_n(&asymb->synced, __ATOMIC_RELAXED) !=
			__atomic_load_n(&asymb->refcnt, __ATOMIC_RELAXED))
		ASYM_BARRIER_RELAX();

	__atomic_store_n(&asymb->synced, 0, __ATOMIC_SEQ_CST);

	return;
}

/**
 * @brief Wait for all workers are synchronized if the period has updated.
 *
 * If all workers are already synchronized, this function will not block.
 *
 * @param asymb
 *	The pointer to an asymmetric barrier object.
 */
static inline void asym_barrier_wait(asym_barrier_t *asymb)
{
	if (ASYM_BARRIER_UNLIKELY(!asymb))
		return;

	if (ASYM_BARRIER_UNLIKELY(
				__atomic_load_n(&asymb->wcount, __ATOMIC_RELAXED)))
	{
		(void)__atomic_sub_fetch(&asymb->wcount, 1, __ATOMIC_ACQUIRE);

		while (__atomic_load_n(&asymb->wcount, __ATOMIC_RELAXED))
			ASYM_BARRIER_RELAX();

		(void)__atomic_add_fetch(&asymb->synced, 1, __ATOMIC_ACQ_REL);

		while (__atomic_load_n(&asymb->synced, __ATOMIC_RELAXED))
			ASYM_BARRIER_RELAX();
	}

	return;
}

#endif /* ASYM_BARRIER_H */
