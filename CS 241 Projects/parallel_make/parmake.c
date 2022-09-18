/**
 * parallel_make
 * CS 241 - Spring 2022
 */

/* ref: https://stackoverflow.com/questions/146924/how-can-i-tell-if-a-given-path-is-a-directory-or-a-file-c-c
 *      https://stackoverflow.com/questions/9376975/c-linux-check-if-file-is-updated-changed-modified
 */

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "./includes/queue.h"
#include "./includes/set.h"

static graph* mk_graph;
static queue* q;
static set* pre_processing_set;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

bool has_cycle(void* rule, set* visited) {
    if (set_contains(visited, rule)) {
        return true;
    }

    /* mark current node as visited */
    set_add(visited, rule);

    /* recursively find whether there exist a cycle */
    vector *vertices = graph_neighbors(mk_graph, rule);
    size_t s = vector_size(vertices);
    for (size_t i = 0; i < s; i++) {
        if (has_cycle(vector_get(vertices, i), visited)) {
            vector_destroy(vertices);
            return true;
        }
    }

    /* mark the current node as not visited */
    set_remove(visited, rule);

    /* add the rule to my pre_processing_vector */
    set_add(pre_processing_set, rule);

    /* return back */
    vector_destroy(vertices);
    return false;
}

void check_file_modification_status(char* name) {
    /* first check whether the rule is on the disk */
    struct stat tat;
    if (!(stat(name, &tat) == 0 && (tat.st_mode & S_IFREG))) {
        return;
    } else {
        vector* v = graph_antineighbors(mk_graph, name);
        VECTOR_FOR_EACH(v, rule,
            struct stat dep;
            rule_t* dependency = graph_get_vertex_value(mk_graph, rule);
            if (dependency->state == -1)
                continue;

            if (stat(dependency->target, &dep) == 0) {
                if (tat.st_mtime < dep.st_mtime) {
                    dependency->state = 1;
                } else {
                    dependency->state = 0;
                }
            } else {
                dependency->state = 0;
            }
        );
        vector_destroy(v);
    }
}

int execute_commands(vector* v) {
    int ret = -1;
    void** cmds = vector_begin(v);
    void** termin = vector_end(v);

    do {
        if (cmds == termin)
            return 1;
        ret = system(*cmds++);
    } while (ret == 0);

    return -1;
}

/*
 * This function add vertex without dependency into the queue
 */
void pre_processing_v1() {
    vector* v = shallow_vector_create();
    SET_FOR_EACH(pre_processing_set, rule,
        if (graph_vertex_degree(mk_graph, rule) == 0) {
            queue_push(q, graph_get_vertex_value(mk_graph, rule));
            vector_push_back(v, rule);
        }
    );

    VECTOR_FOR_EACH(v, ele,
        set_remove(pre_processing_set, ele);
    );
    vector_destroy(v);
}

/*
 * This function marks the state of vertex with failing dependency as -1
 */
void pre_processing_v2(char* name) {
    vector* v = graph_antineighbors(mk_graph, name);
    VECTOR_FOR_EACH(v, rule,
        rule_t* temp = graph_get_vertex_value(mk_graph, rule);
        temp->state = -1;
    );
    vector_destroy(v);
}

void* processor(void* args) {
    rule_t* rule = NULL;
    while ((rule = queue_pull(q))) {
        /* special case: rule is "" */
        if (*rule->target == 0) {
            queue_push(q, NULL);
            return NULL;
        }

        /* try to execute the commands first */
        check_file_modification_status(rule->target);
        if (rule->state == 0) {
            rule->state = execute_commands(rule->commands);
        }

        /* if fails */
        if (rule->state == -1) {
            pre_processing_v2(rule->target);
        }

        /* remove executed rule from the graph */
        graph_remove_vertex(mk_graph, rule->target);

        /* update the q */
        pthread_mutex_lock(&m);
        pre_processing_v1();
        pthread_mutex_unlock(&m);
    }

    /* add NULL as terminator to the queue*/
    queue_push(q, NULL);

    /* exit from the thread */
    return NULL;
}

int parmake(char *makefile, size_t num_threads, char **targets) {
    /* init the global variables */
    mk_graph = parser_parse_makefile(makefile, targets);
    q = queue_create(-1);
    pre_processing_set = string_set_create();

    /* init local variables */
    pthread_t pid[num_threads];

    /* add tasks to my task queue */
    bool cycle = false;
    vector* v = graph_neighbors(mk_graph, "");
    for (void** rule = vector_begin(v); rule != vector_end(v); rule++) {
        set* visited = string_set_create();
        if (has_cycle(*rule, visited)) {
            print_cycle_failure((char*)*rule);
            set_destroy(pre_processing_set);
            cycle = true;
        }
        set_destroy(visited);

        if (cycle)
            break;
    }
    vector_destroy(v);

    /* create threads */
    for (size_t i = 0; i < num_threads; i++) {
        pthread_create(pid + i, NULL, processor, NULL);
    }

    /* pre_processing job */
    if (/*set_cardinality(pre_processing_set) == 0*/ cycle) {
        queue_push(q, NULL);
    } else {
        set_add(pre_processing_set, "");
        pthread_mutex_lock(&m);
        pre_processing_v1();
        pthread_mutex_unlock(&m);
    }

    /* wait for each thread */
    for (size_t i = 0; i < num_threads; i++) {
        pthread_join(pid[i], NULL);
    }
   
    /* release memeory */
    pthread_mutex_destroy(&m);
    set_destroy(pre_processing_set);
    queue_destroy(q);
    graph_destroy(mk_graph);
    return 0;
}
