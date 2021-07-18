#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

#define TEST_ADDR 0x80000000UL

static void handle_map(int sig, siginfo_t *info, void *ucontext) {
    printf("Got SIGSEGV at address: 0x%lx\n",(long) info->si_addr);
    char *vaddr = (char *) mmap((void *) info->si_addr, 1, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_FIXED | MAP_PRIVATE, 0, 0);
    if (!vaddr) {
      perror("mmap");
      exit(1);
    }
}

int main () { 

  /* Register a SIGSYS handler for trapping syscalls */
	struct sigaction act;
	sigset_t mask;
	memset(&act, 0, sizeof(act));
	sigemptyset(&mask);

	act.sa_sigaction = handle_map;
	act.sa_flags = SA_SIGINFO | SA_NODEFER;
	act.sa_mask = mask;

	if (sigaction(SIGSEGV, &act, NULL) == -1) {
		perror("sigaction");
    return 1;
	}

  char *vaddr = (char *)TEST_ADDR;
  *vaddr = 'a';

  printf("*vaddr=%c\n", *vaddr);

  *(vaddr + 4096) = 'b';
  printf("*vaddr=%c\n", *(vaddr + 4096));


  return 0;
}
