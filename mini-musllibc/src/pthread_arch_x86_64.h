typedef unsigned long uintptr_t;

static inline uintptr_t __mini_get_tp()
{
	uintptr_t tp;
	__asm__ ("mov %%fs:0,%0" : "=r" (tp) );
	return tp;
}

#define MC_PC gregs[REG_RIP]