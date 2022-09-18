/**
 * password_cracker
 * CS 241 - Spring 2022
 */
#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "./includes/queue.h"
#include <crypt.h>
#include <string.h>
#include <stdio.h>

typedef struct crypt_data crypt_data;
typedef struct task {
    char* user_name;
    char* hashed_pwd;
    char* known_pwd;
} task_t;

typedef struct processor_task {
    pthread_t id;
    queue* q;
} processor_t;

static int numRecovered, numFailed;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void looping(char* start, char* known_pwd, char* hashed_pwd, int* done, int* hashCount, crypt_data* cdata) {
    if (!start) {
        *done = 1;
        return;
    }
    if (*(start + 1) == 0) {
        for (int i = 0; i < 26; i++) {
            *start = 97 + i;
            const char *hashed = crypt_r(known_pwd, "xx", cdata);
            (*hashCount)++;
            if (strcmp(hashed, hashed_pwd) == 0) {
                *done = 1;
                break;
            }
        }
        return;
    }
    for (int i = 0; i < 26; i++) {
        *start = 97 + i;
        looping(start + 1, known_pwd, hashed_pwd, done, hashCount, cdata);
        if (*done)
            return;
    }
}

void* processor(void* task) {
    processor_t* tsk = (processor_t*) task;
    task_t* t = NULL;

    while ( (t = queue_pull(tsk->q)) ) {
        /* print default message */
        v1_print_thread_start(tsk->id, t->user_name);

        /* calculate number of dots */
        char* start = strstr(t->known_pwd, ".");

        /* set the flag to determine whether the I find the password */
        int done = 0;

        /* initialize crypt_data */
        crypt_data cdata;
        cdata.initialized = 0;

        /* compute hash count and time */
        int hashCount = 0;

        /* crack the password */
        double start_time = getTime();
        looping(start, t->known_pwd, t->hashed_pwd, &done, &hashCount, &cdata);
        double elapsed = getTime() - start_time;

        if (done) {
            pthread_mutex_lock(&m);
            numRecovered++;
            pthread_mutex_unlock(&m);
            done = 0;
        } else {
            pthread_mutex_lock(&m);
            numFailed++;
            pthread_mutex_unlock(&m);
            done = 1;
        }

        /* print the report */
        v1_print_thread_result(tsk->id, t->user_name, t->known_pwd, hashCount, elapsed, done);

        free(t->user_name);
        free(t);
    }

    /* add NULL into queue*/
    queue_push(tsk->q, NULL);

    return NULL;
}

int start(size_t thread_count) {
    /* init data structures */
    pthread_t pid[thread_count];
    processor_t task[thread_count];
    queue* q = queue_create(thread_count);
    for (size_t i = 0; i < thread_count; i++) {
        task[i].id = i + 1;
        task[i].q = q;
    }

    /* create processors */
    for (size_t i = 0; i < thread_count; i++) {
        pthread_create(pid + i, NULL, processor, task + i);
    }

    /* sake of using getline */
    char* pwd = NULL;
    size_t size = 0;

    while (getline(&pwd, &size, stdin) != EOF) {
        pwd[strlen(pwd) - 1] = 0;
        task_t* temp = calloc(1, sizeof(task_t));

        /* user name*/
        temp->user_name = strdup(pwd);

        /* hashed pwd */
        temp->hashed_pwd = strstr(temp->user_name, " ");
        *(temp->hashed_pwd) = 0;
        temp->hashed_pwd = temp->hashed_pwd + 1;

        /* known part of a password */
        temp->known_pwd = strstr(temp->hashed_pwd, " ");
        *(temp->known_pwd) = 0;
        temp->known_pwd = temp->known_pwd + 1;

        /* add the task to the queue */
        queue_push(q, temp);
    }

    /* add NULL as the terminator into the queue */
    queue_push(q, NULL);

    /* keep waiting until all the processors finish its jobs */
    for (size_t i = 0; i < thread_count; i++) {
        pthread_join(pid[i], NULL);
    }

    free(pwd);
    queue_destroy(q);
    pthread_mutex_destroy(&m);

    /* print the final report */
    v1_print_summary(numRecovered, numFailed);

    return 0;
}
