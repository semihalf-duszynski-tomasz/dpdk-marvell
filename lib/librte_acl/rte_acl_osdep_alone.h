/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2014 Intel Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTE_ACL_OSDEP_ALONE_H_
#define _RTE_ACL_OSDEP_ALONE_H_

/**
 * @file
 *
 * RTE ACL OS dependent file.
 * An example how to build/use ACL library standalone
 * (without rest of DPDK).
 * Don't include that file on it's own, use <rte_acl_osdep.h>.
 */

#if (defined(__ICC) || (__GNUC__ == 4 &&  __GNUC_MINOR__ < 4))

#ifdef __SSE__
#include <xmmintrin.h>
#endif

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#if defined(__SSE4_2__) || defined(__SSE4_1__)
#include <smmintrin.h>
#endif

#if defined(__AVX__)
#include <immintrin.h>
#endif

#else

#include <x86intrin.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif

#define	DUMMY_MACRO	do {} while (0)

/*
 * rte_common related.
 */
#define	__rte_unused	__attribute__((__unused__))

#define RTE_PTR_ADD(ptr, x)	((typeof(ptr))((uintptr_t)(ptr) + (x)))

#define	RTE_PTR_ALIGN_FLOOR(ptr, align) \
	(typeof(ptr))((uintptr_t)(ptr) & ~((uintptr_t)(align) - 1))

#define	RTE_PTR_ALIGN_CEIL(ptr, align) \
	RTE_PTR_ALIGN_FLOOR(RTE_PTR_ADD(ptr, (align) - 1), align)

#define	RTE_PTR_ALIGN(ptr, align)	RTE_PTR_ALIGN_CEIL(ptr, align)

#define	RTE_ALIGN_FLOOR(val, align) \
	(typeof(val))((val) & (~((typeof(val))((align) - 1))))

#define	RTE_ALIGN_CEIL(val, align) \
	RTE_ALIGN_FLOOR(((val) + ((typeof(val))(align) - 1)), align)

#define	RTE_ALIGN(ptr, align)	RTE_ALIGN_CEIL(ptr, align)

#define	RTE_MIN(a, b)	({ \
		typeof(a) _a = (a); \
		typeof(b) _b = (b); \
		_a < _b ? _a : _b;   \
	})

#define	RTE_DIM(a)		(sizeof(a) / sizeof((a)[0]))

/**
 * Searches the input parameter for the least significant set bit
 * (starting from zero).
 * If a least significant 1 bit is found, its bit index is returned.
 * If the content of the input parameter is zero, then the content of the return
 * value is undefined.
 * @param v
 *     input parameter, should not be zero.
 * @return
 *     least significant set bit in the input parameter.
 */
static inline uint32_t
rte_bsf32(uint32_t v)
{
	asm("bsf %1,%0"
		: "=r" (v)
		: "rm" (v));
	return v;
}

/*
 * rte_common_vect related.
 */
typedef __m128i xmm_t;

#define	XMM_SIZE	(sizeof(xmm_t))
#define	XMM_MASK	(XMM_SIZE - 1)

typedef union rte_xmm {
	xmm_t    x;
	uint8_t  u8[XMM_SIZE / sizeof(uint8_t)];
	uint16_t u16[XMM_SIZE / sizeof(uint16_t)];
	uint32_t u32[XMM_SIZE / sizeof(uint32_t)];
	uint64_t u64[XMM_SIZE / sizeof(uint64_t)];
	double   pd[XMM_SIZE / sizeof(double)];
} rte_xmm_t;

#ifdef __AVX__

typedef __m256i ymm_t;

#define	YMM_SIZE	(sizeof(ymm_t))
#define	YMM_MASK	(YMM_SIZE - 1)

typedef union rte_ymm {
	ymm_t    y;
	xmm_t    x[YMM_SIZE / sizeof(xmm_t)];
	uint8_t  u8[YMM_SIZE / sizeof(uint8_t)];
	uint16_t u16[YMM_SIZE / sizeof(uint16_t)];
	uint32_t u32[YMM_SIZE / sizeof(uint32_t)];
	uint64_t u64[YMM_SIZE / sizeof(uint64_t)];
	double   pd[YMM_SIZE / sizeof(double)];
} rte_ymm_t;

#endif /* __AVX__ */

#ifdef RTE_ARCH_I686
#define _mm_cvtsi128_si64(a) ({ \
	rte_xmm_t m;            \
	m.x = (a);              \
	(m.u64[0]);             \
})
#endif

/*
 * rte_cycles related.
 */
static inline uint64_t
rte_rdtsc(void)
{
	union {
		uint64_t tsc_64;
		struct {
			uint32_t lo_32;
			uint32_t hi_32;
		};
	} tsc;

	asm volatile("rdtsc" :
		"=a" (tsc.lo_32),
		"=d" (tsc.hi_32));
	return tsc.tsc_64;
}

/*
 * rte_lcore related.
 */
#define rte_lcore_id()	(0)

/*
 * rte_errno related.
 */
#define	rte_errno	errno
#define	E_RTE_NO_TAILQ	(-1)

/*
 * rte_rwlock related.
 */
#define	rte_rwlock_read_lock(x)		DUMMY_MACRO
#define	rte_rwlock_read_unlock(x)	DUMMY_MACRO
#define	rte_rwlock_write_lock(x)	DUMMY_MACRO
#define	rte_rwlock_write_unlock(x)	DUMMY_MACRO

/*
 * rte_memory related.
 */
#define	SOCKET_ID_ANY	-1                  /**< Any NUMA socket. */
#define	RTE_CACHE_LINE_SIZE	64                  /**< Cache line size. */
#define	RTE_CACHE_LINE_MASK	(RTE_CACHE_LINE_SIZE-1) /**< Cache line mask. */

/**
 * Force alignment to cache line.
 */
#define	__rte_cache_aligned	__attribute__((__aligned__(RTE_CACHE_LINE_SIZE)))


/*
 * rte_byteorder related.
 */
#define	rte_le_to_cpu_16(x)	(x)
#define	rte_le_to_cpu_32(x)	(x)

#define rte_cpu_to_be_16(x)	\
	(((x) & UINT8_MAX) << CHAR_BIT | ((x) >> CHAR_BIT & UINT8_MAX))
#define rte_cpu_to_be_32(x)	__builtin_bswap32(x)

/*
 * rte_branch_prediction related.
 */
#ifndef	likely
#define	likely(x)	__builtin_expect((x), 1)
#endif	/* likely */

#ifndef	unlikely
#define	unlikely(x)	__builtin_expect((x), 0)
#endif	/* unlikely */


/*
 * rte_tailq related.
 */

struct rte_tailq_entry {
	TAILQ_ENTRY(rte_tailq_entry) next; /**< Pointer entries for a tailq list
 */
	void *data; /**< Pointer to the data referenced by this tailq entry */
};

static inline void *
rte_dummy_tailq(void)
{
	static __thread TAILQ_HEAD(rte_dummy_head, rte_dummy) dummy_head;
	TAILQ_INIT(&dummy_head);
	return &dummy_head;
}

#define	RTE_TAILQ_LOOKUP_BY_IDX(idx, struct_name)	rte_dummy_tailq()

#define RTE_EAL_TAILQ_REMOVE(idx, type, elm)	DUMMY_MACRO

/*
 * rte_string related
 */
#define	snprintf(str, len, frmt, args...)	snprintf(str, len, frmt, ##args)

/*
 * rte_log related
 */
#define RTE_LOG(l, t, fmt, args...)	printf(fmt, ##args)

/*
 * rte_malloc related
 */
#define	rte_free(x)	free(x)

static inline void *
rte_zmalloc_socket(__rte_unused const char *type, size_t size, unsigned align,
	__rte_unused int socket)
{
	void *ptr;
	int rc;

	align = (align != 0) ? align : RTE_CACHE_LINE_SIZE;
	rc = posix_memalign(&ptr, align, size);
	if (rc != 0) {
		rte_errno = rc;
		return NULL;
	}

	memset(ptr, 0, size);
	return ptr;
}

#define	rte_zmalloc(type, sz, align)	rte_zmalloc_socket(type, sz, align, 0)

/*
 * rte_debug related
 */
#define	rte_panic(fmt, args...)	do {         \
	RTE_LOG(CRIT, EAL, fmt, ##args);     \
	abort();                             \
} while (0)

#define	rte_exit(err, fmt, args...)	do { \
	RTE_LOG(CRIT, EAL, fmt, ##args);     \
	exit(err);                           \
} while (0)

#define	rte_cpu_get_flag_enabled(x)	(0)

#ifdef __cplusplus
}
#endif

#endif /* _RTE_ACL_OSDEP_ALONE_H_ */
