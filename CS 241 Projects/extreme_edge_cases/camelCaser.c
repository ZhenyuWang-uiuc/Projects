/**
 * extreme_edge_cases
 * CS 241 - Spring 2022
 */
#include "camelCaser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define True 1
#define False 0

char **camel_caser(const char *input_str) {
    if (!input_str)
        return NULL;

    // counters
    unsigned punctCounter = 1;
    unsigned pageCounter = 0;
    unsigned slot = 0;
    unsigned counter = 0;

    // flags
    unsigned reallocRequired = False;
    unsigned invalid = True;
    unsigned isStartingWord = True;
    unsigned isCamelCase = False;

    char **ans = calloc(punctCounter, sizeof(char*));
    char buffer[4097];
    memset(buffer, 0, 4097);

    for (const char *start = input_str; *start; start++) {
        invalid = True;
        if (counter == 4096) {
            reallocRequired = True;
            if (++pageCounter == 1) {
                ans = realloc(ans, ++punctCounter * sizeof(char*));
                ans[slot] = calloc(4097, 1);
            } else {
                ans[slot] = realloc(ans[slot], 4096 * pageCounter + 1);
            }
            if (pageCounter == 1)
                strcpy(ans[slot], buffer);
            else
                strcat(ans[slot], buffer);
            counter = 0;
            memset(buffer, 0, 4096);
        }
        if (ispunct( *start )) {
            if (reallocRequired) {
                ans[slot] = realloc(ans[slot], 4096 * pageCounter + counter + 1);
                strcat(ans[slot], buffer);
                reallocRequired = False;
                pageCounter = 0;
            } else {
                ans = realloc(ans, ++punctCounter * sizeof(char*));
                ans[slot] = calloc(counter + 1, 1);
                strcpy(ans[slot], buffer);
            }
            memset(buffer, 0, counter);
            counter = 0;
            slot ++;
            isStartingWord = True;
            isCamelCase = False;
            invalid = False;
        } else if (isspace( *start )) {
            if (!isStartingWord)
                isCamelCase = True;
        } else if (!isspace( *start ) && !isalpha( *start )) {
            buffer[counter++] = *start;
            if (isStartingWord)
                isStartingWord = False;
        } else {
            if (isCamelCase) {
                isCamelCase = False;
                if (*start >= 'a' && *start <= 'z')
                    buffer[counter++] = *start - 32;
                else
                    buffer[counter++] = *start;
            } else {
                if (isStartingWord)
                    isStartingWord = False;
                if (*start >= 'A' && *start <= 'Z')
                    buffer[counter++] = *start + 32;
                else
                    buffer[counter++] = *start;
            }
        }
    }

    if (invalid && pageCounter) {
        free(ans[slot]);
        ans = realloc(ans, (punctCounter - 1) * sizeof (char*));
    }
    ans[slot] = NULL;

    return ans;
}

void destroy(char **result) {
    if (result == NULL)
        return;
    char **tmp = result;
    while (*result) {
        free(*result);
        result++;
    }
    free(tmp);
    return;
}
