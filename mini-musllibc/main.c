#include "include/mini_syscalls.h"
// #include <stdio.h>
#include "include/fcntl.h"
#include <unistd.h>


int main() {
  char buf[1024] = {0};
  
  if (mini_read(1, buf, 6) < 0) {
    return 1;
  }

  buf[5] = '\0';

  int fd;
  if (( fd = mini_open("./test_file", O_CREAT | O_RDWR, 777)) < 0) {
    return 1;
  }

  mini_write(fd, buf, 5);
  return 0;
}