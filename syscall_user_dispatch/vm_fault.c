#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define TEST_VADDR 0x80008000UL

static void handle_segv (int sig, siginfo_t *info, void *ucontext) {
  printf("%d got SIGSEGV at address: 0x%lx\n", getpid(),(unsigned long) info->si_addr);
  kill(getppid(), SIGUSR1);
  exit(0);
}

int main (int argc, char **argv) {

  asm (
        "mov $60, %%rax;"     // exit is 60
        "xor %%rdi, %%rdi;"    // return code 0
        "syscall;"
        :
        :          
  );

  /* Register a SIGSYS handler for trapping syscalls */
	struct sigaction act;
	sigset_t mask;
	memset(&act, 0, sizeof(act));
	sigemptyset(&mask);

  act.sa_sigaction = handle_segv;
	act.sa_flags = SA_SIGINFO | SA_NODEFER;
	act.sa_mask = mask;

	if (sigaction(SIGSEGV, &act, NULL) == -1) {
		perror("sigaction");
    return 1;
	}

  puts("hi");
  char *p = (char*)TEST_VADDR;
  *p = 'a';
  return 0;
}