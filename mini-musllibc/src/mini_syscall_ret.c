#include <errno.h>
#include "internal_syscall.h"

long __mini_syscall_ret(unsigned long r)
{
	if (r > -4096UL) {
		errno = -r;
		return -1;
	}
	return r;
}