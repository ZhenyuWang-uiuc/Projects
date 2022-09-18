/**
 * utilities_unleashed
 * CS 241 - Spring 2022
 */

// the way to calculate time:
// https://stackoverflow.com/questions/3946842/measuring-time-taken-by-a-function-clock-gettime

#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "format.h"

#define NANOSECOND_PER_SECOND 1000000000

int main(int argc, char *argv[]) {
    if (argc < 2)
        print_time_usage();

    struct timespec start, end;
    int status;

    clock_gettime(CLOCK_MONOTONIC, &start);
    pid_t id = fork();
    if (id == -1) {
        print_fork_failed();
    } else if (id == 0) {
        if (argc == 2)
            execlp(argv[1], argv[1], NULL);
        else
            execvp(argv[1], argv + 1);  /* use execvp() rather than execv()! */
        print_exec_failed();
    } else {
        waitpid(id, &status, 0);
        if (WEXITSTATUS(status) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &end);
            display_results(argv, (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / NANOSECOND_PER_SECOND);
        }
    }
    return 0;
}
