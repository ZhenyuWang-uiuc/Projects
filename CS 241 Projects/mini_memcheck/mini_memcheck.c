/**
 * mini_memcheck
 * CS 241 - Spring 2022
 */

// Lab partners: tianyun9, zhenyuw5

#include "mini_memcheck.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

meta_data *head = NULL;
size_t total_memory_requested = 0;
size_t total_memory_freed = 0;
size_t invalid_addresses = 0;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    if (request_size == 0) {
        return NULL;
    }

    meta_data *meta = malloc(sizeof(meta_data) + request_size);
    if (head == NULL) {
        head = meta;
    } else {
        meta_data *tmp = head;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = meta;
    }

    meta->filename = filename;
    meta->instruction = instruction;
    meta->request_size = request_size;
    meta->next = NULL;

    total_memory_requested += request_size;

    return (void*)(meta + 1); // (void*) meta + sizeof(meta_data)
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    if (num_elements == 0 || element_size == 0) {
        return NULL;
    }

    void *ptr = mini_malloc(num_elements * element_size, filename, instruction);
    memset(ptr, 0, num_elements * element_size);

    return ptr;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    if (!payload) {
        return mini_malloc(request_size, filename, instruction);
    } else if (request_size == 0) {
        mini_free(payload);
        return NULL;
    }

    meta_data *tmp = head;
    meta_data *prv = head;
    bool isBadAddressValid = true;

    while (tmp != NULL) {
        if ((void*)(tmp + 1) == payload) {
            isBadAddressValid = false;
            break;
        }
        prv = tmp;
        tmp = tmp->next;
    }

    if (isBadAddressValid) {
        invalid_addresses += 1;
        return NULL;
    }

    meta_data *meta = payload - sizeof(meta_data);
    meta = realloc(meta, sizeof(meta_data) + request_size);
    if (prv == head && !prv->next) {
        head = meta;
    } else {
        prv->next = meta;
    }
    total_memory_freed += (request_size < meta->request_size) ? meta->request_size - request_size : 0;
    total_memory_requested += (request_size >= meta->request_size) ? request_size - meta->request_size : 0;
    meta->request_size = request_size;
    return meta + 1;
}

void mini_free(void *payload) {
    if (payload == NULL) {
        return;
    }

    meta_data *tmp = head;
    meta_data *prv = head;
    bool isBadAddressValid = true;

    while (tmp != NULL) {
        if ((void*)(tmp + 1) == payload) {
            if (tmp == head) {
                head = tmp->next;
            }
            prv->next = tmp->next;
            total_memory_freed += tmp->request_size;
            free(tmp);
            isBadAddressValid = false;
            break;
        }
        prv = tmp;
        tmp = tmp->next;
    }

    if (isBadAddressValid) {
        invalid_addresses += 1;
    }
}
