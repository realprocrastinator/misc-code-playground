// #include "pthread_impl.h"
#include "internal_syscall.h"

hidden long __mini_syscall_cp_c();

static long mini_sccp(syscall_arg_t nr,
                 syscall_arg_t u, syscall_arg_t v, syscall_arg_t w,
                 syscall_arg_t x, syscall_arg_t y, syscall_arg_t z)
{
	return __mini_syscall(nr, u, v, w, x, y, z);
}

weak_alias(mini_sccp, __mini_syscall_cp_c);

long (__mini_syscall_cp)(syscall_arg_t nr,
                    syscall_arg_t u, syscall_arg_t v, syscall_arg_t w,
                    syscall_arg_t x, syscall_arg_t y, syscall_arg_t z)
{
	return __mini_syscall_cp_c(nr, u, v, w, x, y, z);
}
