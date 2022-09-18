/**
 * vector
 * CS 241 - Spring 2022
 */
#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    char *data;
};

sstring *cstr_to_sstring(const char *input) {
    assert(input);
    sstring *s = calloc(1, sizeof(sstring));
    s->data = calloc(strlen(input) + 1, 1);
    strcpy(s->data, input);
    return s;
}

char *sstring_to_cstr(sstring *input) {
    assert(input);
    char *s = calloc(strlen(input->data) + 1, 1);
    strcpy(s, input->data);
    return s;
}

int sstring_append(sstring *this, sstring *addition) {
    assert(this && addition);
    size_t len_of_s1 = strlen(this->data);
    size_t len_of_s2 = strlen(addition->data);
    this->data = realloc(this->data, len_of_s1 + len_of_s2 + 1);
    strcat(this->data, addition->data);
    return len_of_s1 + len_of_s2;
}

vector *sstring_split(sstring *this, char delimiter) {
    assert(this);
    vector* v = string_vector_create();
    char s[strlen(this->data) + 1];
    strcpy(s, this->data);

    char *ptr = s;
    char *head = s;
    while (*ptr) {
        if (*ptr == delimiter) {
            *ptr = 0;
            vector_push_back(v, head);
            head = ptr + 1;
        }
        ptr ++;
    }
    vector_push_back(v, head);
    return v;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    assert(this && target && substitution);
    char *s = strstr(this->data + offset, target);
    if (!s)
        return -1;
    size_t index = s - this->data;
    size_t len_of_this = strlen(this->data);
    size_t len_of_tat = strlen(target);
    size_t len_of_sub = strlen(substitution);
    size_t len_of_newStr = len_of_this + len_of_sub - len_of_tat + 1;

    char tmp[len_of_this - index - len_of_tat + 1];
    memset(tmp, 0, sizeof(tmp));
    strcpy(tmp, this->data + index + len_of_tat);
    this->data = realloc(this->data, len_of_newStr);
    this->data[index] = 0;
    strcat(this->data, substitution);
    strcat(this->data, tmp);
    return 0;
}

char *sstring_slice(sstring *this, int start, int end) {
    assert(this);
    char *s = calloc(end - start + 1, 1);
    for (int i = start; i < end; i++) {
        s[i - start] = this->data[i];
    }
    return s;
}

void sstring_destroy(sstring *this) {
    assert(this);
    free(this->data);
    free(this);
}
