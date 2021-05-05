#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <sched.h>

#define INTERVAL 1
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

static bool if_continue = true;
static int retval = 0;

void handle_sigusr1(int signum) {
  printf("Caught signal %d, going to terminate.\n", signum);
  // terminate the infinite while loop
  if_continue = false;
}

void *do_cnt(void *arg) {
  register uint64_t nsyscalls = 0;
  struct timespec start, end;

  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = handle_sigusr1;

  // install the signal handler
  if (sigaction(SIGUSR1, &sa, NULL) < 0) {
    perror("Failed to register signal handler.");
    retval = errno;
    pthread_exit(&retval);
  }

  // read the start time
  if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
    perror("Failed to get start time.");
    retval = errno;
    pthread_exit(&retval);
  }

  while (if_continue) {
    getpid();
    ++nsyscalls;
  }

  // read the end time
  if (clock_gettime(CLOCK_MONOTONIC, &end) < 0) {
    perror("Failed to get end time.");
    retval = errno;
    pthread_exit(&retval);
  }

  // read the count and store to the vector
  printf("Executed %lu syscalls per second.\n", nsyscalls);

  // nano seconds
  double avg_exetime = 0.0;
  unsigned long long elapsed_ns = GET_ELAPSED_TIME(start, end);
  printf("In total the elapsed time is: %llu ns.\n", elapsed_ns);
  avg_exetime = elapsed_ns / nsyscalls;
  printf("On average, each syscall execution time: %lf ns.\n", avg_exetime);

  pthread_exit(&retval);
}

int main() {
  pthread_t t_id;
  int ret;
  void *status = NULL;

  if ((ret = pthread_create(&t_id, NULL, do_cnt, NULL)) != 0) {
    handle_err_en(ret, "Failed to create pthread.");
  }

  sleep(1);

  // kill the running thread
  if ((ret = pthread_kill(t_id, SIGUSR1)) != 0) {
    handle_err_en(ret, "Failed to kill running thread.");
  }

  if ((ret = pthread_join(t_id, &status)) != 0) {
    handle_err_en(ret, "Failed to join with the thread.");
  }

  printf("Thread terminated :%ld with status %d\n.", t_id, *(int *)status);

  return 0;
}