#include <stdio.h>
#include <stdlib.h>

#define UNUSED __attribute__((unused))

__thread int test;

int main () {
  int *tmp = &test;
  printf("TLS var test is at %p\n", &test);
  test = 1;
  *tmp = 2;
  return 0;
}