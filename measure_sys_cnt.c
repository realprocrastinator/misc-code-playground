#include<stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<stdint.h>
#include<pthread.h>
#include<signal.h>
#include<errno.h>

#define INTERVAL 1
#define handle_err_en(en, msg) \
        do {errno = en; perror(msg), exit(EXIT_FAILURE);} while (0)
#define handle_err(msg) \
        do {perror(msg), exit(EXIT_FAILURE);} while (0)
// in nano seonds resolution
#define ONESEC 1e9

static uint64_t nsyscalls = 0;


void handle_sigusr1(int signum) {
  printf("Caught signal %d, going to terminate.\n", signum);
  pthread_exit(NULL);
}

void *do_cnt (void *arg) {
  pid_t pid __attribute__((unused));

  while (1) {
    pid = getpid();
    ++nsyscalls;
  }

  handle_err("We should never reach here!\n");
}

int main () {
  pthread_t t_id;
  int ret;
  // nano seconds
  double avg_exetime = 0.0;

  signal(SIGUSR1, handle_sigusr1);
  
  if ((ret = pthread_create(&t_id, NULL, do_cnt, NULL)) != 0) {
    handle_err_en(ret, "Failed to create pthread.");
  }

  sleep(1);
  printf("Executed %lu syscalls per secon.\n", nsyscalls);
  avg_exetime = ONESEC / nsyscalls;
  printf("On average, each syscall execution time: %lf ns.\n", avg_exetime);
  // kill the running thread
  if ((ret = pthread_kill(t_id, SIGUSR1)) != 0) {
    handle_err_en(ret, "Failed to kill running thread.");
  }
  
  if ((ret = pthread_join(t_id, NULL)) != 0) {
    handle_err_en(ret, "Failed to join with the thread.");
  }
  printf("Thread terminated :%ld\n", t_id);

  return 0;
}