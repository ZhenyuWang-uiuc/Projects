/**
 * critical_concurrency
 * CS 241 - Spring 2022
 */
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// partners: zhenyuw5, tianyun9

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    /* Your code here */
    struct queue* ret = malloc(sizeof(struct queue));
    if (ret == NULL) {
        return NULL;
    }
    ret->head = NULL;
    ret->tail = NULL;
    ret->size = 0;
    ret->max_size = max_size;
    pthread_cond_init(&ret->cv, NULL);
    pthread_mutex_init(&ret->m, NULL);
    return ret;
}

void queue_destroy(queue *this) {
    /* Your code here */
    queue_node* p = this->head;
    while (p) {
        queue_node* temp = p;
        p = p->next;
        free(temp);
    }
    pthread_mutex_destroy(&this->m);
    pthread_cond_destroy(&this->cv);
    free(this);
}

void queue_push(queue *this, void *data) {
    /* Your code here */
    if (this->max_size <= 0) return;

    pthread_mutex_lock(&this->m);
    while (this->size == this->max_size) {
        pthread_cond_wait(&this->cv, &this->m);
    }
    if (this->size == 0) {
        this->head = calloc(1, sizeof(queue_node));
        this->head->data = data;
        this->tail = this->head;
    } else {
        queue_node *n = calloc(1, sizeof(queue_node));
        n->data = data;
        this->tail->next = n;
        this->tail = n;
    }
    this->size ++;
    pthread_cond_broadcast(&this->cv);
    pthread_mutex_unlock(&this->m);
}

void *queue_pull(queue *this) {
    /* Your code here */
    if (this->max_size <= 0) return NULL;

    void *ret = NULL;
    pthread_mutex_lock(&this->m);
    while (this->size == 0) {
        pthread_cond_wait(&this->cv, &this->m);
    }
    ret = this->head->data;
    queue_node *tmp = this->head;
    this->head = this->head->next;
    tmp->next = NULL;
    free(tmp);
    this->size --;
    pthread_cond_broadcast(&this->cv);
    pthread_mutex_unlock(&this->m);

    return ret;
}
