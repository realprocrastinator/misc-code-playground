#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <sched.h>
#include <assert.h>
#include <setjmp.h>

#define handle_err_en(en, msg) \
  do                           \
  {                            \
    errno = en;                \
    perror(msg);               \
    exit(EXIT_FAILURE);        \
  } while (0)
#define handle_err(msg) \
  do                    \
  {                     \
    perror(msg);        \
    exit(EXIT_FAILURE); \
  } while (0)

// in nano seonds resolution
#define SEC_TO_NS 1e9
#define GET_ELAPSED_TIME(s, e) ((e.tv_sec - s.tv_sec) * SEC_TO_NS + (e.tv_nsec - s.tv_nsec))

static int retval = 0;
static jmp_buf jmpbuf;
static struct timespec time_start;

void print_report(uint64_t nsyscalls, struct timespec* start, struct timespec* finish) {
  printf("Executed total %lu syscalls.\n", nsyscalls);

  // nano seconds
  double avg_exetime = 0.0;
  unsigned long long elapsed_ns = GET_ELAPSED_TIME((*start), (*finish));
  printf("In total the elapsed time is: %llu ns.\n", elapsed_ns);
  avg_exetime = elapsed_ns / nsyscalls;
  printf("On average, each syscall execution time: %lf ns.\n", avg_exetime);
}

void handle_sigusr1(int signum) {
  printf("Caught signal %d, stopping ...\n", signum);
  // R: for debug :)
  printf("Signal handler TID: %d\n", gettid());

  // read counter from register!
  register uint64_t nsyscalls __asm__("r12");
  
  // current time
  struct timespec time_end;
  clock_gettime(CLOCK_MONOTONIC, &time_end);

  //report!
  print_report(nsyscalls, &time_start, &time_end);

  // back to the counter thread so that it can return
  longjmp(jmpbuf, 1);
}

void *do_cnt(void *arg) {
  // store syscall count in r12, because this register is persisted.
  register uint64_t nsyscalls __asm__("r12") = 0;

  // R: for debug :)
  printf("Child thread TID: %d\n", gettid());

  if (setjmp(jmpbuf))
    goto finish;

  // read the start time
  if (clock_gettime(CLOCK_MONOTONIC, &time_start) < 0) {
    perror("Failed to get start time.");
    retval = errno;
    pthread_exit(&retval);
  }

  // R: explicit register declaration (I think this is new in GCC 8)
  // used for both return value and syscall no.
  register uint64_t rax __asm__("rax");
  // R: use unconditional jumps to minimise clock time for checking
  while (1) {
    // R: make inline to avoid measuring stack management
    // on the next loop, rax will become the return value
    rax = __NR_getpid;
    asm volatile (
      "syscall\n\t"
      : "+r" (rax) // "+ is R/W for RAX"
      : // no inputs
      : "rcx", "r11" // these registers are clobbered after syscall
    );
    
    // R: uncomment to see if returned PID is correct
    // printf("My PID: %ld\n", rax);
    // if(nsyscalls > 2)
    //   break;

    // ++nsyscalls
    asm volatile (
      "inc %%r12\n\t"
      : "+r" (nsyscalls)
      :
      :
    );
  }

finish:
  return &retval;
}

int main(int argc, char **argv) {
  // R: for debug :)
  printf("Main thread TID: %d\n", gettid());

  // R: install the signal handler in the main thread.
  // the child thread will inherit this mask.
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = handle_sigusr1;

  if (sigaction(SIGUSR1, &sa, NULL) < 0) {
    perror("Failed to register signal handler.");
    exit(EXIT_FAILURE);
  }

  // interval of measurement
  int interval;
  pthread_t t_id;
  int ret;
  void *status = NULL;
  
  if (argc != 2) {
    fprintf(stderr, "Usage: %s interval of measurement\n",
                       argv[0]);
    exit(EXIT_FAILURE);
  }

  if ((interval = atoi(argv[1])) <= 0 ) {
    fprintf(stderr, "interval of measurement must be a positive number\n");
    exit(EXIT_FAILURE);
  }
  
  if ((ret = pthread_create(&t_id, NULL, do_cnt, NULL)) != 0) {
    handle_err_en(ret, "Failed to create pthread.");
  }

  sleep(interval);

  // kill the running thread
  if ((ret = pthread_kill(t_id, SIGUSR1)) != 0) {
    handle_err_en(ret, "Failed to kill running thread.");
  }

  if ((ret = pthread_join(t_id, &status)) != 0) {
    handle_err_en(ret, "Failed to join with the thread.");
  }

  printf("Thread terminated : %ld with status %d\n.", t_id, *(int *)status);

  return 0;
}