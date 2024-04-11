

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "ThreadFunction.h"

//Our buffer size, for both sent and received messages in their respective buffer
#define MAX_BYTES 2048


/*
 * This needs to be renamed to main if you want to compile it and run it,
 * Args are as follows
 * (name)
 * (server ip)
 * (server port)
 *
 * Name is chosen via the output file name giving during compilation, I am giving mine
 * on my local machine the name telchat
 */
int main(int argc, char *argv[]) {

    //Ensure proper usage of this program
    if (argc < 3) {
        fprintf(stderr, "Usage : telnet serverip port\n");
        exit(EXIT_FAILURE);
    }
    //Our client socket file descriptor and byte total of the message
    int sockFd;
    //The buffer which be used for the message, with max size of MAX_BYTES defined above
    char buf[MAX_BYTES];
    //Our addr info structs, hints which we use to fill serverInfo and then assign to p which will give us our linked list of addresses we can walk to find what we are looking for
    struct addrinfo hints, *serverInfo, *p;
    //Return value of getaddrinfo
    int returnValue;
    //This will be for our reading thread
    pthread_t thread[1];

    //Memory set hints pointed-to address to 0;
    memset(&hints, 0, sizeof hints);
    //For this we'll only take IPv4
    hints.ai_family = AF_UNSPEC;
    //Stream socket
    hints.ai_socktype = SOCK_STREAM;

    //We will check the return value here of getaddr info for getting the server information , if it succeeds it will assign the information we are looking for to the *serverInfo addrinfo struct! You can thank the
    //kernel for that, if error print error
    if ((returnValue = getaddrinfo(argv[1], argv[2], &hints, &serverInfo)) != 0) {
        fprintf(stderr, "getaddrinfo error : %s\n", gai_strerror(returnValue));
        exit(EXIT_FAILURE);
    }

    //Here we will iterate through the linked list by assigning p to the addrinfo struct and walking through it, trying to create a socket and getting the
    //file descriptor from the kernel,
    for (p = serverInfo; p != NULL; p = p->ai_next) {
        //Here we will try to get the socket file dscriptor with the system call, socket, if error then iterate to the next entry in the list and start over
        if ((sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
        //Attempt to connect to the server socket
        if (connect(sockFd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockFd);
            continue;
        }
        //break the loop
        break;
    }
    //If the pointed to entry in the list is NULL, something went wrong and we need to handle this by returning with an error code (non 0)
    if (p == NULL) {
        fprintf(stderr, "Client failed to connect\n");
        _exit(EXIT_FAILURE);
    }

    //We can free serverInfo since we are done with the memory now
    freeaddrinfo(serverInfo);

    //Print that the user has connected properly
    fprintf(stdout, "Successfully connected\n");


    //Here we will spawn a thread which will just be delegated to recving and printing messages from the server, error handling accordingly
    if ((pthread_create(thread, NULL, (void *) &thread_read, &sockFd)) != 0) {
        strerror(errno);
        exit(EXIT_FAILURE);
    }

    //Our main infinite loop
    for (;;) {
        //prevent funny business, clear the memory space
        memset(buf, 0, sizeof buf);

        //fgets provides line-at-a-time input and is suitable for this telnet client since it is a terminal application, this will terminate on newline which is what we are looking for here
        fgets(buf, sizeof(buf), stdin);

        //Get the length of the actual string not the whole buffer
        size_t len = strlen(buf);

        //Null terminate
        buf[len] = '\0';

        //Attempt to send the message, error handle accordingly, need to make sure to use strlen not size of since that causes issues.
        if ((send(sockFd, buf, strlen(buf), 0)) < 0) {
            strerror(errno);
            exit(EXIT_FAILURE);
        }
    }


    close(sockFd);
    return EXIT_SUCCESS;

}