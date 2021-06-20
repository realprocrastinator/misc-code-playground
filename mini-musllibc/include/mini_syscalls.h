#include <unistd.h>
#include "./sys/socket.h"

// I/O operations
ssize_t mini_read(int fd, void *buf, size_t count);
ssize_t mini_write(int fd, const void *buf, size_t count);
int mini_close(int fd);
int mini_open(const char *filename, int flags, ...);
int mini_unlink(const char *path);

// socket operations
int mini_socket (int, int, int);
int mini_bind (int, const struct sockaddr *, socklen_t);
int mini_connect (int, const struct sockaddr *, socklen_t);
int mini_listen (int, int);
int mini_accept (int, struct sockaddr *__restrict, socklen_t *__restrict);

long mini_syscall(long, ...);