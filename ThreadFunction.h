//
// Created by dustyn on 4/11/24.
//
#include "pthread.h"
#ifndef UNIXTELNETCLIENT_THREADFUNCTION_H
#define UNIXTELNETCLIENT_THREADFUNCTION_H
#ifndef BUFSIZE
#define BUFSIZE 1024
#endif
void *thread_read(const int *sockFd);
#endif //UNIXTELNETCLIENT_THREADFUNCTION_H
