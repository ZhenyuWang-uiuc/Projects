/**
 * savvy_scheduler
 * CS 241 - Spring 2022
 */

// partners: zhenyuw5, tianyun9

#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"

/**
 * The struct to hold the information about a given job
 */
typedef struct _job_info {
    int id;

    /* TODO: Add any other information and bookkeeping you need into this
     * struct. */
    double arrived_time;     // fcfs
    double total_time;       // sjf
    double priority;         // pri
    double start_time;       // calculate average times
    double remaining_time;   // psrtf
    double rr_time;          // rr
} job_info;

static double turnaround;
static double total_response;
static double total_waiting;
static size_t num_jobs;

void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    // Put any additional set up code you may need here
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
    // TODO: Implement me!
    if (((job_info*)((job*)(a))->metadata)->arrived_time == ((job_info*)((job*)(b))->metadata)->arrived_time)
        return 0;
    return ((job_info*)((job*)(a))->metadata)->arrived_time < ((job_info*)((job*)(b))->metadata)->arrived_time ? -1 : 1;
}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
    // TODO: Implement me!
    if (((job_info*)((job*)(a))->metadata)->priority == ((job_info*)((job*)(b))->metadata)->priority)
        return break_tie(a, b);
    return ((job_info*)((job*)(a))->metadata)->priority < ((job_info*)((job*)(b))->metadata)->priority ? -1 : 1;
}

int comparer_psrtf(const void *a, const void *b) {
    // TODO: Implement me!
    if (((job_info*)((job*)(a))->metadata)->remaining_time == ((job_info*)((job*)(b))->metadata)->remaining_time)
        return break_tie(a, b);
    return ((job_info*)((job*)(a))->metadata)->remaining_time < ((job_info*)((job*)(b))->metadata)->remaining_time ? -1 : 1;

    // double time_difference = ((job_info*)((job*)(a))->metadata)->arrived_time - ((job_info*)((job*)(b))->metadata)->arrived_time;
    // double remaining = 0;
    // if (time_difference > 0) {  // b starts before a
    //     remaining = ((job_info*)((job*)(b))->metadata)->total_time > time_difference ? ((job_info*)((job*)(b))->metadata)->total_time - time_difference : 0;
    //     if (remaining > ((job_info*)((job*)(a))->metadata)->total_time)
    //         return -1;
    //     return 1;
    // } else if (time_difference < 0) {  // a starts before b
    //     remaining = ((job_info*)((job*)(a))->metadata)->total_time > -time_difference ? ((job_info*)((job*)(a))->metadata)->total_time + time_difference : 0;
    //     if (remaining > ((job_info*)((job*)(b))->metadata)->total_time)
    //         return 1;
    //     return -1;
    // } else {
    //     if (((job_info*)((job*)(a))->metadata)->total_time == ((job_info*)((job*)(b))->metadata)->total_time)
    //         return 0;
    //     return ((job_info*)((job*)(a))->metadata)->total_time > ((job_info*)((job*)(b))->metadata)->total_time ? -1 : 1;
    // }
}

int comparer_rr(const void *a, const void *b) {
    // TODO: Implement me!
    if (b == NULL)
        return 1;
    if (a == NULL)
        return -1;
    // return comparer_fcfs(a, b);
    if (((job_info*)((job*)(a))->metadata)->rr_time == ((job_info*)((job*)(b))->metadata)->rr_time)
        return 0;
    return ((job_info*)((job*)(a))->metadata)->rr_time < ((job_info*)((job*)(b))->metadata)->rr_time ? -1 : 1;
}

int comparer_sjf(const void *a, const void *b) {
    // TODO: Implement me!
    if (((job_info*)((job*)(a))->metadata)->total_time == ((job_info*)((job*)(b))->metadata)->total_time)
        return break_tie(a, b);
    return ((job_info*)((job*)(a))->metadata)->total_time < ((job_info*)((job*)(b))->metadata)->total_time ? -1 : 1;
}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int job_number, double time,
                       scheduler_info *sched_data) {
    // TODO: Implement me!
    newjob->metadata = calloc(sizeof(job_info), 1);
    ((job_info*)newjob->metadata)->id = job_number;
    ((job_info*)newjob->metadata)->arrived_time = time;
    ((job_info*)newjob->metadata)->total_time = sched_data->running_time;
    ((job_info*)newjob->metadata)->priority = sched_data->priority;
    ((job_info*)newjob->metadata)->remaining_time = sched_data->running_time;
    ((job_info*)newjob->metadata)->rr_time = -1;
    ((job_info*)newjob->metadata)->start_time = -1;
    priqueue_offer(&pqueue, newjob);
    num_jobs ++;
    // total_response += sched_data->total_time;
}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    // TODO: Implement me!
    if (job_evicted == NULL) {
        job_info* temp = ((job*) priqueue_peek(&pqueue))->metadata;
        if (temp->start_time == -1) {
            temp->start_time = time;
        }
        return priqueue_peek(&pqueue);
    }

    // if (priqueue_size(&pqueue) == 0)
    //     return NULL;

    job_info* j_info = job_evicted->metadata;
    j_info->remaining_time--;
    
    // Preemptive Priority, Preemptive Least Remaining Time First, Round Robin are preemptive
    if (pqueue_scheme == PPRI || pqueue_scheme == PSRTF || pqueue_scheme == RR) {
        ((job_info*)job_evicted->metadata)->rr_time = time;
        priqueue_offer(&pqueue, priqueue_poll(&pqueue));
        // printf("%d\n", priqueue_offer(&pqueue, priqueue_poll(&pqueue)));
        // total_waiting += time - ((job_info*)priqueue_peek(&pqueue)->metadata)->arrived_time;
        job_info* temp = ((job*) priqueue_peek(&pqueue))->metadata;
        if (temp->start_time == -1) {
            temp->start_time = time;
        }
        return priqueue_peek(&pqueue);
    } else {
        return job_evicted;
    }
}

void scheduler_job_finished(job *job_done, double time) {
    // TODO: Implement me!
    job_info* j_info = job_done->metadata;
    total_waiting += (time - (j_info->arrived_time + j_info->total_time));
    turnaround += time - j_info->arrived_time;
    total_response += j_info->start_time - j_info->arrived_time;
    free(j_info);
    priqueue_poll(&pqueue);
}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    // TODO: Implement me!
    return total_waiting / num_jobs;
}

double scheduler_average_turnaround_time() {
    // TODO: Implement me!
    return turnaround / num_jobs;
}

double scheduler_average_response_time() {
    // TODO: Implement me!
    return total_response / num_jobs;
}

void scheduler_show_queue() {
    // OPTIONAL: Implement this if you need it!
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}
