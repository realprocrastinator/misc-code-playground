#include "../include/unistd.h"
#include "../include/sys/syscall.h"
#include "../include/errno.h"
#include <stdarg.h>
#include "../include/fcntl.h"
#include "../include/mini_syscalls.h"
#include "internal_syscall.h"

ssize_t mini_read(int fd, void *buf, size_t count)
{
	return mini_syscall_cp(SYS_read, fd, buf, count);
}

ssize_t mini_write(int fd, const void *buf, size_t count)
{
	return mini_syscall_cp(SYS_write, fd, buf, count);
}

int mini_close(int fd)
{
	// fd = __aio_close(fd);
	int r = __mini_syscall_cp(SYS_close, fd);
	if (r == -EINTR) r = 0;
	return __mini_syscall_ret(r);
}

int mini_open(const char *filename, int flags, ...)
{
	mode_t mode = 0;

	if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE) {
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

	int fd = __mini_sys_open_cp(filename, flags, mode);
	if (fd>=0 && (flags & O_CLOEXEC))
		__mini_syscall(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC);

	return __mini_syscall_ret(fd);
}

weak_alias(mini_open, mini_open64);

int mini_socket(int domain, int type, int protocol)
{
	int s = __mini_socketcall(socket, domain, type, protocol, 0, 0, 0);
	if ((s==-EINVAL || s==-EPROTONOSUPPORT)
	    && (type&(SOCK_CLOEXEC|SOCK_NONBLOCK))) {
		s = __mini_socketcall(socket, domain,
			type & ~(SOCK_CLOEXEC|SOCK_NONBLOCK),
			protocol, 0, 0, 0);
		if (s < 0) return __mini_syscall_ret(s);
		if (type & SOCK_CLOEXEC)
			__mini_syscall(SYS_fcntl, s, F_SETFD, FD_CLOEXEC);
		if (type & SOCK_NONBLOCK)
			__mini_syscall(SYS_fcntl, s, F_SETFL, O_NONBLOCK);
	}
	return __mini_syscall_ret(s);
}

int mini_bind(int fd, const struct sockaddr *addr, socklen_t len)
{
	return mini_socketcall(bind, fd, addr, len, 0, 0, 0);
}

int mini_connect(int fd, const struct sockaddr *addr, socklen_t len)
{
	return mini_socketcall_cp(connect, fd, addr, len, 0, 0, 0);
}

int mini_listen(int fd, int backlog)
{
	return mini_socketcall(listen, fd, backlog, 0, 0, 0, 0);
}

int mini_accept(int fd, struct sockaddr *restrict addr, socklen_t *restrict len)
{
	return mini_socketcall_cp(accept, fd, addr, len, 0, 0, 0);
}

int mini_unlink(const char *path)
{
#ifdef SYS_unlink
	return mini_syscall(SYS_unlink, path);
#else
	return syscall(SYS_unlinkat, AT_FDCWD, path, 0);
#endif
}
