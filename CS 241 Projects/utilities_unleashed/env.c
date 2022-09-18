/**
 * utilities_unleashed
 * CS 241 - Spring 2022
 */

#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include "format.h"

int check_validity(int argc, char **argv) {
    unsigned isValid = 1;
    unsigned readCmd = 0;
    unsigned hasCmd = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--") == 0) {
            readCmd = 1;
        } else if (readCmd) {
            hasCmd = 1;
            break;
        } else {
            char *value = strstr(argv[i], "=");
            // does key-value pair have '='
            if (!value) {
                isValid = 0;
                break;
            }
            // does key only contain letters, numbers, or underscore characters
            unsigned flag = 0;
            for (char *p = argv[i]; *p != *value; p++) {
                if (!((*p >= 48 && *p <= 57) || (*p >= 65 && *p <= 90) || (*p >= 97 && *p <= 122) || (*p == 95))) {
                    isValid = 0;
                    flag = 1;
                    break;
                }
            }
            if (flag)
                break;
        }
    }
    return isValid && hasCmd;
}

int main(int argc, char *argv[]) {
    if (argc < 2 || !check_validity(argc, argv))
        print_env_usage();

    pid_t id = fork();
    if (id == -1) {
        print_fork_failed();
    } else if (id == 0) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--") == 0) {
                execvp(argv[i + 1], argv + i + 1);
                print_exec_failed();
            } else {
                char *key = strdup(argv[i]);
                char *value = strstr(key, "=");
                int status;
                *value++ = 0;
                if (*value == '%') {
                    status = setenv(key, getenv(value + 1), 0);
                } else {
                    status = setenv(key, value, 0);
                }
                free(key);
                if (status == -1)
                    print_environment_change_failed();
            }
        }
    } else {
        waitpid(id, NULL, 0);
    }
    return 0;
}
