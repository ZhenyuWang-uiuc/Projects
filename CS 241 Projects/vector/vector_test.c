/**
 * vector
 * CS 241 - Spring 2022
 */
#include "vector.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    //vector* v2 = string_vector_create();
    vector* v_int = int_vector_create();

    int array1[1000000];

    for (int i = 0; i < 1000000; i++) {
        array1[i] = i;
        vector_push_back(v_int, array1 + i);
    }

    assert(vector_size(v_int) == 1000000);
    assert(vector_capacity(v_int) == 1048576);
    assert(!vector_empty(v_int));

    assert(vector_front(v_int) == vector_at(v_int, 0));
    assert(vector_back(v_int) == vector_at(v_int, 999999));

    for (int i = 0; i < 1000000; i++) {
        assert(*(int*)(*vector_at(v_int, i)) == i);
        assert(*(int*)(vector_get(v_int, i)) == i);
    }

    vector_resize(v_int, 500000);
    assert(vector_size(v_int) == 500000);
    for (int i = 0; i < 500000; i++) {
        vector_set(v_int, i, array1 + 347);
    }

    for (int i = 0; i < 500000; i++) {
        assert(*(int*)(*vector_at(v_int, i)) == array1[347]);
        assert(*(int*)(vector_get(v_int, i)) == array1[347]);
    }

    assert(vector_begin(v_int) + vector_size(v_int) == vector_end(v_int));

    for (int i = 500000; i < 1000000; i++) {
        vector_push_back(v_int, &i);
    }
    
    for (int i = 0; i < 1000000; i++) {
        if (i < 500000) {
            assert(*(int*)(*vector_at(v_int, i)) == 347);
            assert(*(int*)(vector_get(v_int, i)) == 347);
        } else {
            assert(*(int*)(*vector_at(v_int, i)) == i);
            assert(*(int*)(vector_get(v_int, i)) == i);
        }
    }

    for (int i = 4999; i >= 0; i--) {
        vector_erase(v_int, 4999 - i);
        vector_insert(v_int, 0, &i);
    }

    for (int i = 0; i < 500000; i++) {
        vector_pop_back(v_int);
    }

    assert(vector_size(v_int) == 500000);

    for (int i = 0; i < 500000; i++) {
        if (i < 5000){
            assert(*(int*)(*vector_at(v_int, i)) == i);
            assert(*(int*)(vector_get(v_int, i)) == i);
        } else {
            assert(*(int*)(*vector_at(v_int, i)) == 347);
            assert(*(int*)(vector_get(v_int, i)) == 347);
        }
    }

    vector_clear(v_int);
    assert(vector_size(v_int) == 0);
    assert(vector_capacity(v_int) == 1048576);

    vector_destroy(v_int);

    printf("Done\n");

    return 0;
}
