/**
 * perilous_pointers
 * CS 241 - Spring 2022
 */
#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // first
    first_step(81);

    // second
    int p2 = 132;
    second_step(&p2);

    //third
    int p3 = 8942;
    int *p3_ptr = &p3;
    double_step(&p3_ptr);

    //fourth
    char p4[] = {1, 8, 0, 0, 8, 16, 0, 15, 0, 0 ,0};
    strange_step(p4 + 2);

    // fifth
    empty_step(p4);

    // sixth
    char p6[] = {0, 0, 0, 'u', 0};
    two_step(p6, p6);

    // seventh
    three_step(p6, p6 + 2, p6 + 4);

    // eighth
    step_step_step(p4 + 2, p4 + 2, p4 + 2);

    // ninth
    it_may_be_odd(p4 + 7, 15);

    // tenth
    char p10[] = " ,CS241";
    tok_step(p10);

    //eleventh
    the_end(p4, p4);

    return 0;
}
