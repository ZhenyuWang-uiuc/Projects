/**
 * charming_chatroom
 * CS 241 - Spring 2022
 */

 // partners: zhenyuw5, tianyun9

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes =
        read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    int32_t s = size;
    s = htonl(s);
    ssize_t write_bytes = 
        write_all_to_socket(socket, (char *)&s, MESSAGE_SIZE_DIGITS);
    return write_bytes;
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    size_t ct = 0;
    while (count != ct) {
        ssize_t ret_bytes = read(socket, buffer + ct, 1);
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

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    size_t ct = 0;
    while (count != ct) {
        ssize_t ret_bytes = write(socket, buffer + ct, 1);
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
