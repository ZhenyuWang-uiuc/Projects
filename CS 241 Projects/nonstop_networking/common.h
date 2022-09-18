/**
 * nonstop_networking
 * CS 241 - Spring 2022
 */
#pragma once
#include <stddef.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define LOG(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0);

typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;

int write_file_to_socket(int socket, FILE* f);
int read_file_from_socket(int socket, const char* output, size_t file_size, const char* buffer, size_t buffer_size);
ssize_t read_all_from_socket(int socket, char* buffer, size_t count);
ssize_t write_all_to_socket(int socket, const char* buffer, size_t count);
