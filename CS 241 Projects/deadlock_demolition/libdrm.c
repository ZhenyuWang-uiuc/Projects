/**
 * deadlock_demolition
 * CS 241 - Spring 2022
 */
#include "graph.h"
#include "libdrm.h"
#include "set.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

// partners: zhenyuw5, tianyun9

struct drm_t {
    pthread_mutex_t m;
};

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static graph* g;
static bool INIT_GLOBAL;

bool vector_contains(vector *this, const void *ele)
{
    void **s = vector_begin(this);
    void **e = vector_end(this);
    for (void **it = s; it != e; it++) {
        if (*it == ele) {
            return true;
        }
    }
    return false;
}

bool has_cycle(void* thread_id, vector *v) {
    // if (this graph has been visited)
    if (vector_contains(v, thread_id)) {
        return true;
    }

    // 1. Mark this node as visited
    vector_push_back(v, thread_id);

    // 2. Traverse through all nodes in the reachable_nodes
    // vector *vertices = graph_vertices(g);
    vector *vertices = graph_neighbors(g, thread_id);
    size_t s = vector_size(vertices);
    for (size_t i = 0; i < s; i++) {
        // 3. Call isCyclic() for each node
        // 4. Evaluate the return value of isCyclic()
        if (has_cycle(vector_get(vertices, i), v)) {
            vector_destroy(vertices);
            return true;
        }
    }
    // Nope, this graph is acyclic
    vector_destroy(vertices);
    return false;
}

drm_t *drm_init() {
    if (!INIT_GLOBAL) {
        INIT_GLOBAL = true;
        g = shallow_graph_create();
    }

    drm_t* drm = malloc(sizeof(drm_t));
    pthread_mutex_init(&drm->m, NULL);

    pthread_mutex_lock(&mutex);
    graph_add_vertex(g, drm);
    pthread_mutex_unlock(&mutex);

    return drm;
}

int drm_post(drm_t *drm, pthread_t *thread_id) {
    // post is unlock
    // check if vertex is in graph
    pthread_mutex_lock(&mutex);
    if (graph_contains_vertex(g, thread_id) && graph_adjacent(g, drm, thread_id)) {
        graph_remove_edge(g, drm, thread_id);
        // if edge from drm to thread exists
        pthread_mutex_unlock(&drm->m);
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

int drm_wait(drm_t *drm, pthread_t *thread_id) {
    // wait is lock
    // add thread to graph if not already in graph
    int ret = -1;

    pthread_mutex_lock(&mutex);
    if (!graph_contains_vertex(g, thread_id)) {
        graph_add_vertex(g, thread_id);
    }

    // add edge to graph
    graph_add_edge(g, thread_id, drm);

    // check deadlock
    vector* v = shallow_vector_create();
    if (has_cycle(thread_id, v)) {
        graph_remove_edge(g, thread_id, drm);
        ret = 0;
        /* prevent lock-order-inversion 
         *  |
         *  v
        */
        pthread_mutex_unlock(&mutex);
    } else {
        /* drm is not in used */
        if (!graph_vertex_degree(g, drm)) {
            graph_remove_edge(g, thread_id, drm);
            graph_add_edge(g, drm, thread_id);
            /* prevent lock-order-inversion 
             *  |
             *  v
            */
            pthread_mutex_unlock(&mutex);
            pthread_mutex_lock(&drm->m);
        } else {  /* drm is in used */
            pthread_mutex_unlock(&mutex);
            pthread_mutex_lock(&drm->m);

            /* modify the RAG */
            pthread_mutex_lock(&mutex);
            graph_remove_edge(g, thread_id, drm);
            graph_add_edge(g, drm, thread_id);
            /* prevent lock-order-inversion 
             *  |
             *  v
            */
            pthread_mutex_unlock(&mutex);
        }
        ret = 1;
    }
    /* prevent lock-order-inversion 
     *  |
     *  v
     * pthread_mutex_unlock(&mutex);
    */
    vector_destroy(v);
    return ret;
}

void drm_destroy(drm_t *drm) {
    if (INIT_GLOBAL) {
        INIT_GLOBAL = 0;
        pthread_mutex_destroy(&mutex);
        graph_destroy(g);
    }
    pthread_mutex_destroy(&drm->m);
    free(drm);
}