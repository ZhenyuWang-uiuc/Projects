/**
 * nonstop_networking
 * CS 241 - Spring 2022
 */
#include <stdio.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "./includes/vector.h"
#include "common.h"
#include "format.h"

static vector* list;

static char template[] = "XXXXXX";
static char* tmp_dirname;
static char path[1024] = ".";
static char header[1024];

static FILE* request_file;

static struct addrinfo hints, *ret;
static bool endSession;

static struct epoll_event array[128];
static struct epoll_event events[128];

int open_server(char*);
void fill_in_path(char*);
void rm_file(char*);
void close_server(int);
int file_contains(char*);

int main(int argc, char **argv) {
    /* set signal handler */
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = close_server;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGPIPE, &act, NULL);

    /* create a temp dir */
    tmp_dirname = mkdtemp(template);
    fill_in_path(tmp_dirname);

    /* create a list to track filename */
    list = string_vector_create();

    /* open the server */
    int sockfd = open_server(argv[1]);

    /* create a epoll */
    int epollfd = epoll_create(128);
    struct epoll_event ev_server;
    memset(&ev_server, 0, sizeof(struct epoll_event));
    ev_server.events = EPOLLIN;
    ev_server.data.fd = sockfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev_server);

    int index = 0;
    while (!endSession) {
        int num_events = epoll_wait(epollfd, array, 128, 100);
        for (int i = 0; i < num_events; i++) {
            int fd = array[i].data.fd;
            int event = array[i].events;

            if (fd == sockfd) {
                /* accept */
                int clientfd = accept(sockfd, NULL, NULL);

                /* create a client event */
                events[index].events = EPOLLIN | EPOLLONESHOT;
                events[index].data.fd = clientfd;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, events + index++);
            } else if (event & EPOLLIN) {
                /* init header */
                memset(header, 0, 1024);
                read_all_from_socket(fd, header, 1024);

                /* parse header */
                char* method = header;
                char* filename = NULL;
                size_t file_size = 0;
                char* temp = (char*)&file_size;
                char* sep = strstr(header, " ");
                if (sep != NULL) {
                    *sep = 0;
                    filename = sep + 1;
                    sep = strstr(sep + 1, "\n");
                    *sep = 0;
                    for (int i = 7; i >= 0; i--)
                        temp[i] = *(sep + 1 + i);

                    sep = sep + 9;  // may contain file content;
                }

                /* init path template */
                *(path + 8) = 0;

                /* read and check which method user requests */
                if (strcmp(method, "GET") == 0 || strcmp(method, "DELETE") == 0) {
                    /* try to open the file */
                    fill_in_path(filename);
                    request_file = fopen(path, "r");

                    /* process the request */
                    if (request_file == NULL) { 
                        write_all_to_socket(fd, "ERROR\n", 6);
                        write_all_to_socket(fd, err_no_such_file, 13);
                    } else {
                        write_all_to_socket(fd, "OK\n", 3);
                        if (*header == 'G') {  // write file size
                            /* get file size */
                            fseek(request_file, 0L, SEEK_END);
                            size_t file_size = ftell(request_file);
                            rewind(request_file);

                            /* write to the client */
                            write_all_to_socket(fd, (char*)&file_size, sizeof(file_size));
                            write_file_to_socket(fd, request_file);
                        } else {  // delete file
                            unlink(path);
                            rm_file(path);
                            fclose(request_file);
                        }
                    }
                } else if (strcmp(method, "PUT") == 0 && strlen(filename) < 255) {
                    /* fill in the path template */
                    fill_in_path(filename);

                    /* read from the client */
                    int ret = read_file_from_socket(fd, path, file_size, sep, 1024 - (int)(sep - header));

                    if (ret == -3 || ret == -4) {
                        write_all_to_socket(fd, "ERROR\n", 6);
                        write_all_to_socket(fd, err_bad_file_size, 16);
                        unlink(path);
                    } else if (ret == 0) {
                        write_all_to_socket(fd, "OK\n", 3);
                        if (!file_contains(path))
                            vector_push_back(list, path);
                    }
                } else if (strcmp(header, "LIST\n") == 0) {
                    write_all_to_socket(fd, "OK\n", 3);

                    /* calculate file size and write it to socket */
                    size_t total_size = 0;
                    for (size_t i = 0; i < vector_size(list); i++)
                        total_size += strlen(*vector_at(list, i)) - 8;
                    total_size = total_size ? total_size - 1 : 0;
                    write_all_to_socket(fd, (char*)&total_size, sizeof(total_size));

                    /* write filename to the socket */
                    for (size_t i = 0; i < vector_size(list); i++) {
                        write_all_to_socket(fd, *vector_at(list, i) + 9, strlen(*vector_at(list, i)) - 9);
                        if (i != vector_size(list) - 1) {
                            write_all_to_socket(fd, "\n", 1);
                        }
                    }
                } else {
                    write_all_to_socket(fd, "ERROR\n", 6);
                    write_all_to_socket(fd, err_bad_request, 12);
                }

                /* half close the client socket*/
                shutdown(fd, SHUT_WR);

                /* rm the fd from epoll */
                epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                index --;
            }
        }
    }

    /* release memory */
    for (size_t i = 0; i < vector_size(list); i++)
        unlink(*vector_at(list, i));
    close(sockfd);
    close(epollfd);
    vector_destroy(list);
    rmdir(tmp_dirname);
}

int open_server(char* port) {
    /* step 0: get my ip address */
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &hints, &ret);

    /* step 1: create a socket */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* step 2: set socket option */
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    /* step 3: bind */
    bind(sockfd, ret->ai_addr, ret->ai_addrlen);

    /* step 4: listen */
    listen(sockfd, 128);

    /* step 5: release addrinfo memory */
    freeaddrinfo(ret);

    return sockfd;
}

void fill_in_path(char* name) {
    strcat(path, "/");
    strcat(path, name);
}

void rm_file(char* filename) {
    for (size_t i = 0; i < vector_size(list); i++) {
        if (strcmp(filename, *vector_at(list, i)) == 0) {
            vector_erase(list, i);
            break;
        }
    }
}

void close_server(int signal) {
    if (signal == SIGINT) {
        endSession = 1;
    }
}

int file_contains(char* filename) {
    for (size_t i = 0; i < vector_size(list); i++) {
        if (strcmp(filename, *vector_at(list, i)) == 0) {
            return 1;
        }
    }
    return 0;
}