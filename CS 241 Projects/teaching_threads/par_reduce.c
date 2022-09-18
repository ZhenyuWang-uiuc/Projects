/**
 * teaching_threads
 * CS 241 - Spring 2022
 */

// team members: zhenyuw5, tianyun9

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

// ref: https://stackoverflow.com/questions/22751762/how-to-make-compiler-not-show-int-to-void-pointer-cast-warnings

/* You might need a struct for each task ... */
typedef struct task {
    int *list;
    size_t list_len;
    reducer reduce_func;
    int base_case;
    int result;
} task;

/* You should create a start routine for your threads. */
void* evaluation(void *t)
{
    size_t len = ((task*)t)->list_len;
    int *list = ((task*)t)->list;
    int result = ((task*)t)->base_case;

    for (size_t i = 0; i < len; i+=1)
        result = ((task*)t)->reduce_func(result, list[i]);

    ((task*)t)->result = result;
    return NULL;  // equals to pthread_exit()
}

int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {

    if (num_threads >= list_len) {
        num_threads = list_len;
    }
    
    task *arr = calloc(sizeof(task), num_threads);
    pthread_t pid[num_threads];
    size_t elements_per_thread = list_len / num_threads;
    int result = base_case;

    if (list_len % num_threads != 0) {
        for (size_t i = 0; i < list_len % num_threads; i++) {
            result = reduce_func(result, list[i]);
        }
        list = list + list_len % num_threads;
    }

    for (size_t i = 0; i < num_threads; i++) {
        arr[i].list_len = elements_per_thread;
        arr[i].list = list + elements_per_thread * i;
        arr[i].reduce_func = reduce_func;
        arr[i].base_case = base_case;
        pthread_create(&pid[i], NULL, evaluation, arr + i);
    }

    for (size_t id = 0; id < num_threads; id++) {
        pthread_join(pid[id], NULL);
    }

    for (size_t i = 0; i < num_threads; i++) {
        result = reduce_func(result, arr[i].result);
    }

    free(arr);
    return result;
}
