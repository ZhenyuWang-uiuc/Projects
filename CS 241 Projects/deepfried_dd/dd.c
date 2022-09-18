/**
 * deepfried_dd
 * CS 241 - Spring 2022
 */

// partners: zhenyuw5, tianyun9

// ref: https://stackoverflow.com/questions/295994/what-is-the-rationale-for-fread-fwrite-taking-size-and-count-as-arguments/296018#296018

#include "format.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define NANOSECOND_PER_SECOND 1000000000L

extern char* optarg;

static size_t full_blocks_in;
static size_t partial_blocks_in;
static size_t full_blocks_out;
static size_t partial_blocks_out;
static size_t total_bytes_copied;

static double time_elapsed;
static struct timespec start, end;

static FILE *i;
static FILE *o ;

static size_t block_size = 512;
static ssize_t copied_block = -1;
static size_t i_skip_block = 0;  // number of blocks to skip at the start of the input file 
static size_t o_skip_block = 0;  // number of blocks to skip at the start of the output file

static int flag;

void close_files(FILE*, FILE*);
void SIGUSR1_handler();
void print_report();

int main(int argc, char **argv) {
    // set signal handlder
    signal(SIGUSR1, SIGUSR1_handler);

    // set default input and output
    i = stdin;
    o = stdout;

    // if option char is followed by a colon, the option requires an argument
    int opt = 0;
    while ((opt = getopt(argc, argv, "i:o:b:c:p:k:")) != -1) { 
        switch (opt) {
            case 'i':
                i = fopen(optarg, "r");
                if (i == NULL) {
                    print_invalid_input(optarg);
                    close_files(i, o);
                    exit(1);
                }
                break;
            case 'o':
                o = fopen(optarg, "r+");
                if (o == NULL && ((o = fopen(optarg, "w")) == NULL)) {
                    print_invalid_output(optarg);
                    close_files(i, o);
                    exit(1);
                }
                break;
            case 'b':
                block_size = atoi(optarg);
                break;
            case 'c':
                copied_block = atoi(optarg);
                break;
            case 'p':
                i_skip_block = atoi(optarg);
                break;
            case 'k':
                o_skip_block = atoi(optarg);
                break;
            default:
                exit(1);
        }
    }

    if (i != stdin)
        fseek(i, i_skip_block * block_size, SEEK_SET);
    if (o != stdout && o_skip_block)
        fseek(o, o_skip_block * block_size, SEEK_SET);

    clock_gettime(CLOCK_REALTIME, &start);
    char block[block_size];
    while (feof(i) == 0 && copied_block) {
        size_t num_bytes_read = fread(block, 1, block_size, i);
        if (num_bytes_read == block_size) {
            full_blocks_in ++;
        } else if (num_bytes_read) {
            partial_blocks_in ++;
        }

        size_t num_bytes_write = fwrite(block, 1, num_bytes_read, o);
        if (num_bytes_write == block_size) {
            full_blocks_out ++;
        } else if (num_bytes_write) {
            partial_blocks_out ++;
        }

        total_bytes_copied += num_bytes_write;
        copied_block --;

        if (flag) {
            print_report();
            flag = 0;
        }
    }
    print_report();

    close_files(i, o);
    return 0;
}

void print_report() {
    clock_gettime(CLOCK_REALTIME, &end);
    time_elapsed = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / NANOSECOND_PER_SECOND;
    print_status_report(full_blocks_in, partial_blocks_in, full_blocks_out, partial_blocks_out, total_bytes_copied, time_elapsed);
}

void SIGUSR1_handler() {
    flag = 1;
}

void close_files(FILE* in, FILE* out) {
    if (in && in != stdin) fclose(in);
    if (out && out != stdout) fclose(out);
}
