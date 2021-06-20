#define _BSD_SOURCE
#include "../include/unistd.h"
#include "internal_syscall.h"
#include "../include/stdarg.h"

#undef syscall

long mini_syscall(long n, ...)
{
	va_list ap;
	syscall_arg_t a,b,c,d,e,f;
	va_start(ap, n);
	a=va_arg(ap, syscall_arg_t);
	b=va_arg(ap, syscall_arg_t);
	c=va_arg(ap, syscall_arg_t);
	d=va_arg(ap, syscall_arg_t);
	e=va_arg(ap, syscall_arg_t);
	f=va_arg(ap, syscall_arg_t);
	va_end(ap);
	return __mini_syscall_ret(__mini_syscall(n,a,b,c,d,e,f));
}