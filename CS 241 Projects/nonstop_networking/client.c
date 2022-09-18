/**
 * nonstop_networking
 * CS 241 - Spring 2022
 */

// ref: https://stackoverflow.com/questions/4160347/close-vs-shutdown-socket

#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "common.h"

static struct addrinfo hints, *ret;
static char header_arr[1024];
static FILE* f;
static size_t header_len;

char **parse_args(int argc, char **argv);
verb check_args(char **args);
int connect_to_server(const char *host, const char *port);
void fill_in_header(char* array, const char* method, const char* filename);
void close_server_connection(int socket);

int main(int argc, char **argv) {
    /* check commands' validity */
    char** commands = parse_args(argc, argv);
    verb method = check_args(commands);

    /* connect to the server */
    int sockfd = connect_to_server(commands[0], commands[1]);

    /* fill in the request header */
    fill_in_header(header_arr, commands[2], commands[3] ? commands[3] : "");
    if (strcmp(commands[2], "PUT") == 0) {
        /* try to open the file */
        if ((f = fopen(commands[4], "r")) == NULL)
            exit(1);
        /**
         * 1. get file size 
         * 2. write the file size to the header
         * 3. set file pointer to the beginning
         * 4. update header_len
         */
        fseek(f, 0L, SEEK_END);
        size_t file_size = ftell(f);
        char* temp = (char*)&file_size;
        for (int i = 7; i >= 0; i--)
            header_arr[header_len + i] = temp[i];
        rewind(f);
        header_len += 8;
    }

    /* write request to the server */
    if (write_all_to_socket(sockfd, header_arr, header_len > 1024 ? 1024 : header_len) == -1) {
        perror("write()");
        close_server_connection(sockfd);
        exit(1);
    }
    
    if (strcmp(commands[2], "PUT") == 0) {
        int ret = write_file_to_socket(sockfd, f);
        if (ret == -1) {
            perror("write()");
            close_server_connection(sockfd);
            exit(1);
        }
    }

    /* 'half close' the communication */
    shutdown(sockfd, SHUT_WR);

    /* read the response header */
    memset(header_arr, 0, 1024);
    if (read_all_from_socket(sockfd, header_arr, 3) == -1) {
        perror("read()");
        close_server_connection(sockfd);
        exit(1);
    }

    /* parse the response status code */
    if (strcmp(header_arr, "OK\n") && strcmp(header_arr, "ERR")) {
        print_invalid_response();
    } else if (strcmp(header_arr, "ERR") == 0) {
        if (read_all_from_socket(sockfd, header_arr + 3, 1021) == -1) {
            perror("read()");
            close_server_connection(sockfd);
            exit(1);
        }
        char *p = strstr(header_arr + 6, "\n");
        *p = 0;
        print_error_message(header_arr + 6);
    } else if (method == 0 || method == 3) {  // GET and LIST
        int ret = read_file_from_socket(sockfd, commands[4], 0, NULL, 0);
        if (ret == -1) {
            perror("read()");
            close_server_connection(sockfd);
            exit(1);
        } else if (ret == -3) {
            print_received_too_much_data();
        } else if (ret == -4) {
            print_too_little_data();
        }
    } else if (method == 1 || method == 2) {  // PUT and DELETE
        print_success();
    }

    /* close the connection */
    close_server_connection(sockfd);
    print_connection_closed();
    free(commands);
    return 0;
}

void close_server_connection(int socket) {
    close(socket);
}

void fill_in_header(char* array, const char* method, const char* filename) {
    strcpy(array, method);
    if (*filename != 0) {
        strcat(array, " ");
        strcat(array, filename);
    }
    strcat(array, "\n");
    header_len = strlen(array);
}

/**
 * Sets up a connection to a chatroom server and returns
 * the file descriptor associated with the connection.
 *
 * host - Server to connect to.
 * port - Port to connect to server on.
 *
 * Returns integer of valid file descriptor, or exit(1) on failure.
 */
int connect_to_server(const char *host, const char *port) {
    /* step 0 : init variables */
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int sockfd = -1;

    /* step 1 : get address info */
    int result = getaddrinfo(host, port, &hints, &ret);
    if (result) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        exit(1);
    }

    /* step 2 : create a socket */    
    if ((sockfd = socket(ret->ai_family, ret->ai_socktype, ret->ai_protocol)) == -1) {
        freeaddrinfo(ret);
        perror("socket failed: ");
        exit(1);
    }
    
    /* step 3: connect */
    if(connect(sockfd, ret->ai_addr, ret->ai_addrlen) == -1) {
        freeaddrinfo(ret);
        perror("connect failed: ");
        exit(1);
    }

    freeaddrinfo(ret);
    return sockfd;
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
