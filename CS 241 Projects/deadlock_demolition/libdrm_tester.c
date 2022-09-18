/**
 * deadlock_demolition
 * CS 241 - Spring 2022
 */
#include "libdrm.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _obj {
    drm_t *drm_1;
    drm_t *drm_2;
}obj;

void *func_1(void *object)
{
    pthread_t id = pthread_self();
    printf("thread 1\n");
    printf("thread 1 %d\n", drm_wait( ((obj*)object)->drm_1, &id ));
    printf("thread 1 %d\n", drm_wait( ((obj*)object)->drm_2, &id ));
    printf("thread 1 %d\n", drm_post( ((obj*)object)->drm_2, &id ));
    printf("thread 1 %d\n", drm_post( ((obj*)object)->drm_1, &id ));
    return NULL;
}

void *func_2(void *object)
{
    pthread_t id = pthread_self();
    printf("thread 2\n");
    printf("thread 2 %d\n", drm_wait( ((obj*)object)->drm_2, &id ));
    printf("thread 2 %d\n", drm_wait( ((obj*)object)->drm_1, &id ));
    printf("thread 2 %d\n", drm_post( ((obj*)object)->drm_1, &id ));
    printf("thread 2 %d\n", drm_post( ((obj*)object)->drm_2, &id ));
    return NULL;
}

int main() {
    drm_t *drm_1 = drm_init();
    drm_t *drm_2 = drm_init();

    pthread_t id[2];
    obj o[2] = {{.drm_1 = drm_1, .drm_2 = drm_2}, 
                {.drm_1 = drm_1, .drm_2 = drm_2}};

    pthread_create(id + 0, NULL, func_1, o + 0);
    pthread_create(id + 1, NULL, func_2, o + 1);

    for (int i = 0; i < 2; i++) {
        pthread_join(id[i], NULL);
    }

    drm_destroy(drm_1);
    drm_destroy(drm_2);

    return 0;
}
