#ifndef BING_PROXY_INCLUDE_H
#define BING_PROXY_INCLUDE_H

#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <stdio.h>

// void Sem_init(sem_t *sem, int pshared, unsigned int value) {
//     if (sem_init(sem, pshared, value) < 0)
//         unix_error("Sem_init error");
// }


void unix_error(char *msg) /* Unix-style error */
{
    printf("%s: %s\n", msg, strerror(errno));
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}



void Sem_init(sem_t *sem, int pshared, unsigned int value) {
    if (sem_init(sem, pshared, value) < 0)
        unix_error("Sem init error");
}

void P(sem_t *sem) {
    if (sem_wait(sem) < 0)
        unix_error("P error");
}

void V(sem_t *sem) {
    if (sem_post(sem) < 0)
        unix_error("V error");
}



#endif