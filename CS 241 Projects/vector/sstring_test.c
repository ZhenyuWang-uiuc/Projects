/**
 * vector
 * CS 241 - Spring 2022
 */
#include "sstring.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    sstring *s1 = cstr_to_sstring("abcdeeeeeeeeeeeeeeeefg");
    sstring *s2 = cstr_to_sstring("This is a {} day, {}!");
    char *s3;
    vector* v = sstring_split(s1, 'd');

    void **begin = vector_begin(v);
    void **end = vector_end(v);
    for (; begin != end; begin++)
        printf("\"%s\"\n", *begin);

    sstring_substitute(s2, 18, "{}", "friend");
    sstring_substitute(s2, 0, "{}", "good");
    s3 = sstring_to_cstr(s2);
    printf("%s\n", s3);

    vector_destroy(v);
    sstring_destroy(s1);
    sstring_destroy(s2);
    free(s3);
    return 0;
}
