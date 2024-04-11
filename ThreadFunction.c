//
// Created by dustyn on 4/11/24.
//

#include <stdio.h>
#include <unistd.h>
#include "ThreadFunction.h"
#include "sys/socket.h"
#include "stdlib.h"
#include "memory.h"
#include "errno.h"


/*
 * This is a thread function which takes the sockFd of the server socket and it just sleeps on recv forever until new messages come in then repeats. We do not need poll here since this threads entire purpose is to recv on the socket
 * so there is no worries about blocking. It can block all day who cares, I sure don't. Buffer is zerod each time to make sure that there is no funny business in the message printing to stdout via printf. We will also provide
 * a proper server disconnect message.
 *
 * In summary: We spawn a reader thread for recving incoming messages on our telnet client.
 */
#define MAX_BYTES 1024
void *thread_read(const int *sockFd) {

    //This will hold the bytes read which will tell us if there is A: an error B: a server disconnect and C: where to stop reading in the buffer, we will null terminate that spot in the buffer
    ssize_t bytes_read;

    //Our separate buffer for reading
    char read_buf[MAX_BYTES];

    //Infinite loop
    for (;;) {

        //Memset to prevent any funny business , just in case
        memset(read_buf, 0, sizeof read_buf);

        //Attempt recv, error handle appropriately or handle a disconnect
        bytes_read = recv(*sockFd, read_buf, sizeof read_buf, 0);

        if (bytes_read < 0) {
            strerror(errno);
            exit(EXIT_FAILURE);
        }

        else if (bytes_read == 0) {

            //We'll call _exit directly here you know why? Because I feel like it.
            //We have to flush the stdout stream manually since the system call does not flush the buffer
            fprintf(stdout, "Server disconnected\n");

            fflush(stdout);

            _exit(EXIT_SUCCESS);
        }

        //Null terminate that bitch
        read_buf[bytes_read] = '\0';
        //print the message to the users console
        printf("%s", read_buf);
    }
}
