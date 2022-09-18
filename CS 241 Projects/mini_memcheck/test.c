/**
 * mini_memcheck
 * CS 241 - Spring 2022
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    void *p1 = malloc(30);
    void *p2 = malloc(20);
    void *p3 = malloc(50);
    p2 = realloc(p2, 4096);
    p2 = realloc(p2, 20);
    p2 = realloc(p2, 4096);
    p2 = realloc(p2, 4);
    free(p1);
    free(p2);
    free(p3);
    free(&p3);
    return 0;
}