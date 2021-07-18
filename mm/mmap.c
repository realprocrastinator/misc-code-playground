#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define TEST_ADDR 0x80000000UL

int main () {
  /**
   * test how to map a file to the memory
   */
  
  int fd = open("./test", O_CREAT | O_RDWR, S_IRWXU | S_IRGRP | S_IRWXO);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  /* get the file stat */
  struct stat statbuf;
  if (fstat(fd, &statbuf) < 0) {
    perror("fstat");
  }

  printf("file size is: %ld\n", statbuf.st_size);

  char *vaddr = (char *) mmap((void *) TEST_ADDR, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, fd, 0);
  if (!vaddr) {
    perror("mmap");
  }

  printf("file mapped to address %lx\n", (unsigned long) vaddr);
  
  /* write to the file using mmio */
  if (statbuf.st_size > 0) {
    sprintf((char *)vaddr, "Hello World");
    vaddr[800] = 'c';
  }

  return 0;
}