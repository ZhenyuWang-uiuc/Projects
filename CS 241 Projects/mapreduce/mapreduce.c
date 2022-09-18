/**
 * mapreduce
 * CS 241 - Spring 2022
 */

// partners: zhenyuw5, tianyun9

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdbool.h>
#include "utils.h"

#define ENOUGH ((CHAR_BIT * sizeof(int) - 1) / 3 + 2)

static int num_mappers;

void close_unused_descriptors(int index, bool write_to_reducer, bool is_read, int fds[][2]) {
    if (!write_to_reducer) {
        close(fds[num_mappers][1]);
    }
    close(fds[num_mappers][0]);

    for (int i = 0; i < num_mappers; i++) {
        if (i == index) {
            if (is_read) {
                close(fds[i][1]);
            } else {
                close(fds[i][0]);
            }
        } else {
            close(fds[i][0]);
            close(fds[i][1]);
        }
    }
}

int main(int argc, char **argv) {
    // Create an input pipe for each mapper.
    num_mappers = atoi(argv[5]);
    int fds[num_mappers + 1][2];
    for (int i = 0; i < num_mappers + 1; i++) {
        pipe(fds[i]);
    }

    // Open the output file.
    FILE *out = fopen(argv[2], "w");

    // Start the reducer process.
    pid_t reducer_id = fork();
    if (reducer_id == 0) {
        /* close unused pipes */
        for (int i = 0; i < num_mappers; i++) {
            close(fds[i][0]);
            close(fds[i][1]);
        }
        close(fds[num_mappers][1]);

        /* read from the pipe */
        dup2(fds[num_mappers][0], fileno(stdin));

        /* write stdout to the file */
        dup2(fileno(out), fileno(stdout));

        /* execute the command */
        execlp(argv[4], argv[4], NULL);

        /* print error message */
        fprintf(stderr, "./myreducer fails");
        exit(-1);
    }

    for (int i = 0; i < num_mappers; i++) {
        // Start all the mapper processes.
        if (fork() == 0) {
            /* close unused pipes */
            close_unused_descriptors(i, true, true, fds);

            /* read from the pipe */
            dup2(fds[i][0], fileno(stdin));

            /* write stdout to the pipe */
            dup2(fds[num_mappers][1], fileno(stdout));

            /* execute the command */
            execlp(argv[3], argv[3], NULL);

            /* print error message */
            fprintf(stderr, "./mymapper fails");
            exit(-1);
        }
    }

    for (int i = 0; i < num_mappers; i++) {
        // Start a splitter process for each mapper.
        if (fork() == 0) {
            /* close unused pipes */
            close_unused_descriptors(i, false, false, fds);

            /* move stdout to pipe */
            dup2(fds[i][1], fileno(stdout));

            /* parse the index */
            char idx[ENOUGH];
            sprintf(idx, "%d", i);

            /* execute the command */
            execlp("./splitter", "./splitter", argv[1], argv[5], idx, (char*)NULL);

            /* print error message */
            fprintf(stderr, "./splitter fails");
            exit(-1);
        }
    }

    for (int i = 0; i < num_mappers + 1; i++) {
        close(fds[i][0]);
        close(fds[i][1]);
    }

    // Wait for the reducer to finish.
    int *status = NULL;
    waitpid(reducer_id, status, 0);

    // reap processes to avoid zombie processes
    while (wait(NULL) != -1);

    // Print nonzero subprocess exit codes.
    print_nonzero_exit_status(argv[4], WEXITSTATUS(*status));

    // Count the number of lines in the output file.
    print_num_lines(argv[2]);
    fclose(out);

    return 0;
}
