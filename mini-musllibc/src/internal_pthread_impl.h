#ifndef _PTHREAD_IMPL_H
#define _PTHREAD_IMPL_H

#ifndef __NEED_locale_t
#define __NEED_locale_t
#endif

#include "../include/unistd.h"
#include "../include/bits/alltypes.h"
// #include <pthread.h>
// #include <signal.h>
// #include <errno.h>
// #include <limits.h>
// #include <sys/mman.h>
// #include "libc.h"
// #include "syscall.h"
// #include "atomic.h"
// #include "futex.h"

#include "pthread_arch_x86_64.h"

#define pthread __pthread

typedef unsigned long uintptr_t;

struct pthread {
	/* Part 1 -- these fields may be external or
	 * internal (accessed via asm) ABI. Do not change. */
	struct pthread *self;
#ifndef TLS_ABOVE_TP
	uintptr_t *dtv;
#endif
	struct pthread *prev, *next; /* non-ABI */
	uintptr_t sysinfo;
#ifndef TLS_ABOVE_TP
#ifdef CANARY_PAD
	uintptr_t canary_pad;
#endif
	uintptr_t canary;
#endif

	/* Part 2 -- implementation details, non-ABI. */
	int tid;
	int errno_val;
	volatile int detach_state;
	volatile int cancel;
	volatile unsigned char canceldisable, cancelasync;
	unsigned char tsd_used:1;
	unsigned char dlerror_flag:1;
	unsigned char *map_base;
	size_t map_size;
	void *stack;
	size_t stack_size;
	size_t guard_size;
	void *result;
	struct __ptcb *cancelbuf;
	void **tsd;
	struct {
		volatile void *volatile head;
		long off;
		volatile void *volatile pending;
	} robust_list;
	int h_errno_val;
	volatile int timer_id;
	locale_t locale;
	volatile int killlock[1];
	char *dlerror_buf;
	void *stdio_locks;

	/* Part 3 -- the positions of these fields relative to
	 * the end of the structure is external and internal ABI. */
#ifdef TLS_ABOVE_TP
	uintptr_t canary;
	uintptr_t *dtv;
#endif
};

#ifdef TLS_ABOVE_TP
#define TP_ADJ(p) ((char *)(p) + sizeof(struct pthread) + TP_OFFSET)
#define __pthread_self() ((pthread_t)(__get_tp() - sizeof(struct __pthread) - TP_OFFSET))
#else
#define TP_ADJ(p) (p)
#define __mini_pthread_self() ((pthread_t)__mini_get_tp())
#endif

#endif