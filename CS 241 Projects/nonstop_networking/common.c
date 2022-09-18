/**
 * nonstop_networking
 * CS 241 - Spring 2022
 */
#include "common.h"

/*
 *  0 - success
 * -1 - write fails
 */
int write_file_to_socket(int socket, FILE* f) {
    char arr[4096];
    while (feof(f) == 0) {
        size_t num_bytes_read = fread(arr, 1, 4096, f);
        if (write_all_to_socket(socket, arr, num_bytes_read) == -1)
            return -1;
    }
    fclose(f);
    return 0;
}

/*
 *  0 - success
 * -1 - read fails
 * -3 - too much
 * -4 - too little
 */
int read_file_from_socket(int socket, const char* output, size_t file_size, const char* buffer, size_t buffer_size) {
    /* try to create a file */
    FILE *f = fopen(output, "w");
    if (f == NULL)
        f = stdout;

    /* get file size */
    if (file_size == 0 && read_all_from_socket(socket, (char*)&file_size, sizeof(file_size)) == -1)
        return -1;

    /* write the file content from the server */
    char arr[4096];
    size_t ct = 0;

    // write any possible buffer to the file
    if (buffer)
        ct += fwrite(buffer, 1, file_size > buffer_size ? buffer_size : file_size, f);

    while (1) {
        ssize_t ret_bytes = read_all_from_socket(socket, arr, 4096);
        if (ret_bytes == -1) {
            return -1;
        } else if (ret_bytes == 0) {
            break;
        }

        fwrite(arr, 1, ret_bytes, f);
        ct += ret_bytes;
    }

    if (ct > file_size) {
        return -3;
    } else if (ct < file_size) {
        return -4;
    }
    
    if (f != stdout)
        fclose(f);

    return 0;
}

ssize_t read_all_from_socket(int socket, char* buffer, size_t count) {
    size_t ct = 0;
    while (count != ct) {
        ssize_t ret_bytes = read(socket, buffer + ct, count - ct);
        if (ret_bytes == 0) {
            return ct;
        } else if (ret_bytes > 0) {
            ct += ret_bytes;
        } else if (ret_bytes == -1 && errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }
    return ct;
}

ssize_t write_all_to_socket(int socket, const char* buffer, size_t count) {
    size_t ct = 0;
    while (count != ct) {
        ssize_t ret_bytes = write(socket, buffer + ct, count - ct);
        if (ret_bytes > 0) {
            ct += ret_bytes;
        } else if (ret_bytes == -1 && errno == EINTR) {
            continue;
        } else {            
            return -1;
        }
    }
    return ct;
}
