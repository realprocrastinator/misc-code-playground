#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include <sys/socket.h>
#include <sys/un.h>

#include "../include/sys/socket.h"
#include "../include/mini_syscalls.h"
#include "../include/unistd.h"

#define SOCKET_NAME "/tmp/uds-test.socket"
#define BUFFER_SIZE 12

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  int ret;
  int data_socket;
  char buffer[BUFFER_SIZE];

  // create local socket
  data_socket = mini_socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (data_socket < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));

  // connect socket to the socket addr
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

  ret = mini_connect(data_socket, (const struct sockaddr*) &addr, sizeof(addr));
  if (ret < 0) {
    fprintf(stderr, "The server is down.\n");
    exit(EXIT_FAILURE);
  }

  // send arguments
  for (int i = 1; i < argc; ++i) {
    ret = mini_write(data_socket, argv[i], strlen(argv[i]) + 1);
    if (ret == -1) {
      perror("write failed");
      break;
    }
  }

  // request result
  strcpy(buffer, "END");
  ret = mini_write(data_socket, buffer, strlen(buffer) + 1);
  if (ret < 0) {
    perror("write failed");
    exit(EXIT_FAILURE);
  }

  // receive result
  ret = mini_read(data_socket, buffer, sizeof(buffer));
  if (ret == -1) {
    perror("read failed");
    exit(EXIT_FAILURE);
  }

  // ensure the buffer is null terminated
  buffer[BUFFER_SIZE - 1] = 0;

  printf("Result = %s\n", buffer);

  // close the socket
  mini_close(data_socket);
  exit(EXIT_SUCCESS);
}