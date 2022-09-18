/**
 * critical_concurrency
 * CS 241 - Spring 2022
 */
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"

int main(int argc, char **argv) {
    queue *q = queue_create(5);
    queue_push(q, "1");
    queue_push(q, "2");
    queue_push(q, "3");
    queue_push(q, "4");
    queue_push(q, "5");
    // queue_push(q, "6");

    for (size_t i = 0; i < 5; i++) {
        char *s = queue_pull(q);
        printf("%p %s\n", s, s);
    }
    // queue_pull(q);

    queue_destroy(q);

    return 0;
}
