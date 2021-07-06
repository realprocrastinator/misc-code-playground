#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>


#define SOCKET_NAME "/tmp/uds-test.socket"
#define BUFFER_SIZE 12

// int my_socket(int domain, int type, int protocal) {
//     int fd;
    
//     asm (
//         "mov %%rax, %%rsi;"
//         "mov $41, %%rax;"   // syscall number must be put in %rax
//         "mov $1, %%rdi;"   // folow the calling convention of x86, this will be the AF_UNIX
//         "mov $5, %%rsi;"   // this will be the SOCK_SEQPACKET
//         "mov $0, %%rdx;"  // protocol
//         "syscall;"
//         : "=r" (fd)
//         :
//         : 
//     );

//     return fd;
// }

/*
* File server.c
*/

int
main(int argc, char *argv[])
{
    struct sockaddr_un name;
    int down_flag = 0;
    int ret;
    int connection_socket;
    int data_socket;
    int result;
    char buffer[BUFFER_SIZE];

    /* Create local socket. */

    connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
    * For portability clear the whole structure, since some
    * implementations have additional (nonstandard) fields in
    * the structure.
    */

    memset(&name, 0, sizeof(name));

    /* Bind socket to socket name. */

    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    ret = bind(connection_socket, (const struct sockaddr *) &name,
              sizeof(name));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /*
    * Prepare for accepting connections. The backlog size is set
    * to 20. So while one request is being processed other requests
    * can be waiting.
    */

    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /* This is the main loop for handling connections. */

    for (;;) {

        /* Wait for incoming connection. */

        data_socket = accept(connection_socket, NULL, NULL);
        if (data_socket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        result = 0;
        for (;;) {

            /* Wait for next data packet. */

            puts("Waiting...");
            ret = read(data_socket, buffer, sizeof(buffer));
            if (ret == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            /* Ensure buffer is 0-terminated. */

            buffer[sizeof(buffer) - 1] = 0;

            printf("Data received: %s\n", buffer);

            // /* Handle commands. */

            if (!strncmp(buffer, "DOWN", sizeof(buffer))) {
                down_flag = 1;
            }

            if (!strncmp(buffer, "END", sizeof(buffer))) {
                break;
            }

            /* Add received summand. */

            result += atoi(buffer); 
            break;
        }

        /* Send result. */

        // sprintf(buffer, "%d", result);
        ret = write(data_socket, buffer, sizeof(buffer));
        if (ret == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        /* Close socket. */

        close(data_socket);

        /* Quit on DOWN command. */

        if (down_flag) {
            break;
        }
    }

    close(connection_socket);

    /* Unlink the socket. */

    unlink(SOCKET_NAME);

    exit(EXIT_SUCCESS);
}