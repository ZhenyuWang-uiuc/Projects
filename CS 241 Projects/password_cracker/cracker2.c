/**
 * password_cracker
 * CS 241 - Spring 2022
 */
#include "cracker2.h"
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
    int thread_count;
} processor_t;

static pthread_barrier_t b_start, b_end;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static queue* q;

static char* pwd;
static int found, hashCount;

void init_known_pwd(long start_index, char* known_pwd) {
    if (!known_pwd)
        return;

    if (start_index == 0) {
        for (char* temp = known_pwd; *temp; temp++)
            *temp = 'a';
        return;
    }

    int start_pos = strlen(known_pwd) - 1;
    int int_part = 0;
    double decimal_part = 0;
    double temp = start_index;

    while (temp > 26) {
        *(known_pwd + start_pos) = 'a';
        start_pos -= 1;
        temp /= 26;
    }
    
    *(known_pwd + start_pos) = 'a' + temp;
    int_part = temp;
    decimal_part = temp - int_part;
    if (decimal_part)
        *(known_pwd + start_pos + 1) = 'a' + (int)(decimal_part * 26);
}

void password_cracker(char* hashed_pwd, char* cp_known_pwd, long count, int id, crypt_data* cdata) {
    int hash_ct = 0;  // local hashCount
    int has_printed = 0;  // flag that tracks whether the thread prints the result

    for (long i = 0; i < count; i++) {
        /* check whether there exists a thread that already find the password */
        if (found) {
            v2_print_thread_result(id, hash_ct, 1);
            free(cp_known_pwd);
            has_printed = 1;
            break;
        }

        /* comparison */
        const char *hashed = crypt_r(cp_known_pwd, "xx", cdata);
        hash_ct++;
        if (strcmp(hashed, hashed_pwd) == 0) {
            found = 1;
            pwd = cp_known_pwd;
            v2_print_thread_result(id, hash_ct, 0);
            has_printed = 1;
            break;
        }
        incrementString(cp_known_pwd);
    }

    pthread_mutex_lock(&m);
    hashCount += hash_ct;
    pthread_mutex_unlock(&m);

    if (!has_printed) {
        v2_print_thread_result(id, hash_ct, 2);
        free(cp_known_pwd);
    }
}

void* processor(void* tsk) {
    long start_index = 0;
    long count = 0;
    processor_t* task = tsk;
    task_t* t = NULL;

    while ( (t = queue_pull(q)) ) {
        /* print default message */
        pthread_barrier_wait(&b_start);

        /* calculate the subtasks */
        char* cp_known_pwd = strdup(t->known_pwd);
        char* start = strstr(cp_known_pwd, ".");
        getSubrange(strlen(cp_known_pwd) - getPrefixLength(cp_known_pwd), task->thread_count, task->id, &start_index, &count);
        init_known_pwd(start_index, start);
        v2_print_thread_start(task->id, t->user_name, start_index, cp_known_pwd);

        /* initialize crypt_data */
        crypt_data cdata;
        cdata.initialized = 0;

        /* try to crack the password */
        password_cracker(t->hashed_pwd, cp_known_pwd, count, task->id, &cdata);
        pthread_barrier_wait(&b_end);
    }

    /* add NULL into queue*/
    queue_push(q, NULL);

    return NULL;
}

int start(size_t thread_count) {
    pthread_barrier_init(&b_start, NULL, thread_count);
    pthread_barrier_init(&b_end, NULL, thread_count + 1);
    q = queue_create(thread_count);
    processor_t task[thread_count];
    pthread_t pid[thread_count];
    for (size_t i = 0; i < thread_count; i++) {
        task[i].id = i + 1;
        task[i].thread_count = thread_count;
    }

    /* create processors */
    for (size_t i = 0; i < thread_count; i++) {
        pthread_create(pid + i, NULL, processor, task + i);
    }

    /* sake of using getline */
    char* temp_pwd = NULL;
    size_t size = 0;
    while (getline(&temp_pwd, &size, stdin) != EOF) {
        temp_pwd[strlen(temp_pwd) - 1] = 0;
        task_t* temp = calloc(1, sizeof(task_t));

        /* user name */
        temp->user_name = strdup(temp_pwd);

        /* hashed pwd and print the user name */
        temp->hashed_pwd = strstr(temp->user_name, " ");
        *(temp->hashed_pwd) = 0;
        temp->hashed_pwd = temp->hashed_pwd + 1;

        v2_print_start_user(temp->user_name);
        
        /* known part of a password */
        temp->known_pwd = strstr(temp->hashed_pwd, " ");
        *(temp->known_pwd) = 0;
        temp->known_pwd = temp->known_pwd + 1;

        /* add the task to the queue */
        for (size_t i = 0; i < thread_count; i++) {
            queue_push(q, temp);
        }

        /* wait for other threads finishing their tasks */
        double start_time = getTime();
        double start_cpu_time = getCPUTime();
        pthread_barrier_wait(&b_end);
        double elapsed = getTime() - start_time;
        double total_cpu_time = getCPUTime() - start_cpu_time;

        /* print the summary and set global varables to default value */
        v2_print_summary(temp->user_name, pwd, hashCount, elapsed, total_cpu_time, (found) ? 0 : 1);
        hashCount = found = 0;

        /* release temporary variable */
        if (pwd) {
            free(pwd); pwd = NULL;
        }
        free(temp->user_name);
        free(temp);
    }

    /* add NULL as the terminator into the queue */
    queue_push(q, NULL);

    /* keep waiting until all the processors finish its jobs */
    for (size_t i = 0; i < thread_count; i++) {
        pthread_join(pid[i], NULL);
    }

    free(temp_pwd);
    queue_destroy(q);
    pthread_barrier_destroy(&b_start);
    pthread_barrier_destroy(&b_end);
    pthread_mutex_destroy(&m);
    return 0;
}
