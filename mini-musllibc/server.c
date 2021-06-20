#include "../include/sys/socket.h"
#include "../include/mini_syscalls.h"
#include "../include/unistd.h"

// glibc include
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define SOCKET_NAME "/tmp/uds-test.socket"
#define BUFFER_SIZE 12


int main () {
  // hdl recv(client)
  struct sockaddr_un name;
  int down_flag = 0;
  int ret;
  int connection_socket;
  int data_socket;
  int result;
  char buffer[BUFFER_SIZE];

  // create local socket
  connection_socket = mini_socket(AF_UNIX, SOCK_SEQPACKET, 0);
  
  if (connection_socket == -1) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  // clean the whole structure
  memset(&name, 0, sizeof(name));

  // bind socket to the socket name
  name.sun_family = AF_UNIX;
  strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

  ret = mini_bind(connection_socket, (const struct sockaddr *)&name, sizeof(name));
  if (ret == -1) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // prepare for acceptin connections. The backlog size is set to 20. So while one request is being processed other requests can be waiting.

  ret = mini_listen(connection_socket, 20);
  if (ret == -1) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  // this is the main loop for handling the connections
  for (;;) {
    // wait for incoming connections
    data_socket = mini_accept(connection_socket, NULL, NULL);
    if (data_socket == -1) {
      perror("accept failed");
      exit(EXIT_FAILURE);
    }

    result = 0;
    for (;;) {
      // wait for the next data packet
      ret = mini_read(data_socket, buffer, sizeof(buffer));
      if (ret == -1) {
        perror("read failed");
        exit(EXIT_FAILURE);
      }

      // ensure the buffer is 0 terminated
      buffer[sizeof(buffer) - 1] = 0;

      // handle cmds
      if (!strncmp(buffer, "END", sizeof(buffer))) {
        break;
      }

      if (!strncmp(buffer, "DOWN", sizeof(buffer))) {
        down_flag = 1;
      }

      if (!down_flag) {
        result += atoi(buffer);
      }
    }

    sprintf(buffer, "%d", result);
    ret = mini_write(data_socket, buffer, sizeof(buffer));
    if (ret == -1) {
      perror("write failed");
      exit(EXIT_FAILURE);
    } 

    // for (int i = 0; i < 1000000000; ++i);

    // close the socket
    mini_close(data_socket);

    // check if this is the end

    if (down_flag) {
      break;
    }

  }

  close(connection_socket);
  puts("Server closed everything.");

  // unlink the socket
  unlink(SOCKET_NAME);
  exit(EXIT_SUCCESS);
}