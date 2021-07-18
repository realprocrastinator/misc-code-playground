/* 
 * This is a small program to test Syscall User Dispatch Feature in Linux 5.11 or * newer version 
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/prctl.h>
#include <stdint.h>
#include <asm/prctl.h>
#include <elf.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

// write to that memory will definitelt fail
#define INVALID_MEMORY 0x900000
#define TEST_MEMSZ 0x1000
#define BUFSZ 1024

// glibc memory region
static unsigned long glibc_start;
static unsigned long glibc_end;

/* Control variable for switching wheter to allow or block the syscall */
static uint8_t selector = SYSCALL_DISPATCH_FILTER_BLOCK;

static void handle_syscall (int sig, siginfo_t *info, void *ucontext) {
  if (sig == SIGSYS) {
    puts("Catch a syscall from 'blocked' memory region!");
  }
}

int main() {
  // Register a SIGSYS handler for trapping syscalls
	struct sigaction act;
	sigset_t mask;
	memset(&act, 0, sizeof(act));
	sigemptyset(&mask);

	act.sa_sigaction = handle_syscall;
	act.sa_flags = SA_SIGINFO | SA_NODEFER;
	act.sa_mask = mask;

	if (sigaction(SIGSYS, &act, NULL) == -1) {
		perror("sigaction");
    return 1;
	}

	// `switch off` to allow the syscalls in all the regions except glibc's region.
  selector = SYSCALL_DISPATCH_FILTER_ALLOW;

	// find the memory region which loads the glibc
	FILE *f = fopen("/proc/self/maps", "r");
	if (!f) {
		perror("fopen");
		return 1;
	}

	char buf[BUFSZ];
	while (fgets(buf, sizeof(buf), f)) {
		unsigned long start, end;
		char mode[5];
		unsigned long offset;
		char device[8];
		char inode[12];
		char path[256];

		int ret;
		if ((ret = sscanf(buf, "%lx-%lx %4s %lx %7s %11s %255s\n", &start, &end, mode, &offset, device, inode, path)) == EOF) {
			fprintf(stderr, "parse /proc/self/maps failed\n");
			return 1;
		}

		// found the text section of glibc
		if (strcmp(path, "/usr/lib/x86_64-linux-gnu/libc-2.33.so") == 0 && strcmp(mode, "r-xp") == 0) {
			glibc_start = start;
			glibc_end = end;
		}
	}

	// close the file
	fclose(f);

	if (!glibc_start || !glibc_end) {
		fprintf(stderr, "glibc not loaded!\n");
		return 1;
	}

  if (prctl(PR_SET_SYSCALL_USER_DISPATCH, PR_SYS_DISPATCH_ON, glibc_start, glibc_end, &selector)) {
				perror("prctl");
				return 1;
	}

	// `switch on` to block the syscall in all the regions except glibc's region
  selector = SYSCALL_DISPATCH_FILTER_BLOCK;

	// map a memory region within the blocked area for testing
	printf("Invoking sbrk sycall from the always 'allowed' memory region.\n");
  char *test_memory = malloc(TEST_MEMSZ);
  test_memory[0] = 'a';

	// exit syscall from the block memory region, expects SIGSYS triggered  
  printf("Testing involking syscall from the text section directly instead of via glibc.\n");
  asm (
        "mov $60, %%rax;"     // exit is 60
        "xor %%rdi, %%rdi;"    // return code 0
        "syscall;"
        :
        :          
  );

	printf("Testing accessing memory mapped in the memory region.\n");
  test_memory[0] = 'b';

	// The following code will manually trigger the segmentation fault and see what will happen with SUD enabled

	// test_memory = (char*) INVALID_MEMORY;
	// *test_memory = 'a';

	return 0;
}