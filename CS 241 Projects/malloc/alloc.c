/**
 * malloc
 * CS 241 - Spring 2022
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _metadata {
    size_t size;
    struct _metadata *next;
    struct _metadata *prev;
} meta_t;

static meta_t *head;  // linked list head
static meta_t *tail;  // linked list tail
static size_t cfree;

void remove_meta(meta_t *cur)
{
    cfree --;

    meta_t *p_cur = cur->prev;
    meta_t *n_cur = cur->next;
    /* case 1 */
    if (p_cur) {
        p_cur->next = n_cur;
    } else {
        head = n_cur;
        if (n_cur) n_cur->prev = NULL;
    }
    /* case 2 */   
    if (n_cur) {
        n_cur->prev = p_cur;
    } else {
        tail = p_cur;
        if (p_cur) p_cur->next = NULL;
    }
}

meta_t *find(size_t size)
{
    for (meta_t *h = head; h; h = h->next) {
        if (h->size >= size && h->size <= size + 280) {
            remove_meta(h);
            return h;
        } else if (h->size > size + 280) {
            meta_t *new_meta = (void*)(h + 1) + size;
            /* update new meta */
            new_meta->size = h->size - 24 - size;
            new_meta->next = h->next;
            new_meta->prev = h->prev;
            /* update the old linked list */
            if (h->next) h->next->prev = new_meta;
            if (h->prev) h->prev->next = new_meta;
            h->size = size;
            h->next = NULL;
            h->prev = NULL;
            /* special case */
            if (!new_meta->next) tail = new_meta;
            if (!new_meta->prev) head = new_meta;
            return h;
        }
    }
    return NULL;
}

void insert_meta(void *ptr)
{
    cfree ++;

    meta_t *m = ((meta_t*) ptr) - 1;
    /* case 1: head is NULL */
    if (!head) { 
        tail = head = m;
        return;
    }
    /* case 2: m < head */
    if (m < head) {
        m->next = head; head->prev = m; head = m;
        return;
    }
    /* case 3: m > tail */
    if (m > tail) {
        tail->next = m; m->prev = tail; tail = m;
        return;
    }
    /* case 4: m should place between tail and head */
    for (meta_t *tmp = head; tmp; tmp = tmp->next) {
        if (m < tmp) {
            m->prev = tmp->prev;
            m->next = tmp;
            tmp->prev->next = m;
            tmp->prev = m;
            return;
        }
    }
}

void free(void *ptr)
{
    /* update the linked list */
    insert_meta(ptr);
    meta_t *m = ((meta_t*) ptr) - 1;
    meta_t *prev = m->prev;
    meta_t *next = m->next;
    /* check one previous block */
    if (prev && (void*)(prev + 1) + prev->size == m) {
        prev->size += 24 + m->size;
        prev->next = next;
        if (next) next->prev = prev;
        m = prev;
        cfree --;
    }
    /* check one backward block */
    if (next && (void*)(m + 1) + m->size == next) {
        m->size += 24 + next->size;
        m->next = next->next;
        if (next->next) next->next->prev = m;
        cfree --;
    }
    /* special case */
    if (!m->next) tail = m;
}

void *realloc(void *ptr, size_t size)
{
    if (!ptr) {
        return malloc(size);
    } else if (!size) {
        free(ptr);
        return NULL;
    }

    // 
    meta_t *m = ((meta_t*) ptr) - 1;

    // if the request size less than meta->size, return ptr
    if (m->size >= size && m->size <= size + 280) return ptr;
    if (size <= 65536 && m->size > size + 280) {
        meta_t *new_meta = ptr + size;
        /* update new meta */
        new_meta->size = m->size - 24 - size;
        new_meta->prev = NULL;
        new_meta->next = NULL;
        /* update old meta */
        m->size = size;
        free(new_meta + 1);
        return ptr;
    }

    // allocate the new pointer
    void *newptr = malloc(size);
    if (newptr) {
        memcpy(newptr, ptr, size < m->size ? size : m->size);
        free(ptr);
    }

    return newptr;
}

void *calloc(size_t num, size_t size)
{
    if (num == 0 || size == 0) return NULL;

    void *ptr = malloc(num * size);
    memset(ptr, 0, num * size);
    return ptr;
}

void *malloc(size_t size)
{
    // if size is 0, return NULL
    if (size == 0) return NULL;

    // memory alignment - round up to 16 bytes
    // size = (size % sizeof(meta_t) == 0) ? size : size - size % sizeof(meta_t) + sizeof(meta_t);

    if (head && cfree) {
        meta_t *used_m = find(size);
        if (used_m) { return used_m + 1; }
    }

    /* default approach */
    meta_t *m = sbrk(24 + size);

    // if sbrk fails, returns NULL
    if (m == (void*)-1) return NULL;

    // update the the metadata
    m->size = size; m->next = NULL; m->prev = NULL;

    return m + 1;
}
