/**
 * This program tends to test a few things:  
 * 1) can parent catch child's signal?
 * 2) can prctl apply cross address space?
 * 3) how does execv work?
 * @retval None
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <asm/prctl.h>
#include <sys/prctl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <stdint.h>
#include <asm/prctl.h>

static int selector = SYSCALL_DISPATCH_FILTER_BLOCK;

static void handle_segv (int sig, siginfo_t *info, void *ucontext) {
  printf("%d got SIGSEGV at address: 0x%lx\n", getpid(),(unsigned long) info->si_addr);
  exit(0);
}

static void handle_child_segv (int sig, siginfo_t *info, void *ucontext) {
  printf("Got segfault signal from child");
}

static int child_status;

#define TEST_VADDR 0x80008000UL

int main () {

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

  memset(&act, 0, sizeof(act));
	sigemptyset(&mask);

  act.sa_sigaction = handle_child_segv;
	act.sa_flags = SA_SIGINFO | SA_NODEFER;
	act.sa_mask = mask;

	if (sigaction(SIGUSR1, &act, NULL) == -1) {
		perror("sigaction");
    return 1;
	}
  
  // test if a parent program capture access child's signal
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
  } else if (pid) {
    // parent
    printf("Child has pid %d.\n", pid);
    // char *p = (char*)TEST_VADDR;
    // *p = 'a';

    // need to use wait to check the exit status of child process
    wait(&child_status);

    if (WIFSIGNALED(child_status)) {
      puts("Child terminated by signal");
      if (WTERMSIG(child_status) == SIGSEGV) {
        puts("Child terminated by sigsegv");
      } else if (WTERMSIG(child_status) == SIGSYS)
      {
        puts("Child terminated by sigsys");
      }
      
    }
  } else {
    // child
    // char *p = (char*)TEST_VADDR;
    // *p = 'a';

    // only apply to its own address space
    if (prctl(PR_SET_SYSCALL_USER_DISPATCH, PR_SYS_DISPATCH_ON, 0, 0, &selector) < 0 ) {
      perror("prctl");
      return 1;
    }

    char * const argv[] = {"hi"};
    // char * const envp[1];
    if (execve("./vm_fault", argv, NULL) < 0) {
      perror("execve");
      return 1;
    }
  }

  return 0;
}