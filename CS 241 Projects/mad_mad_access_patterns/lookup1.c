/**
 * mad_mad_access_patterns
 * CS 241 - Spring 2022
 */

 // partners: tianyun9, zhenyuw5

#include "tree.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/

void searchTree(FILE* f, uint32_t offset, char* search_word) {
    if (offset == 0) {
        printNotFound(search_word);
        return;
    }
        
    // locate the node
    fseek(f, offset, SEEK_SET);

    uint32_t left_child = 0;
    uint32_t right_child = 0;
    uint32_t count = 0;
    float price = 0;

    fread((char*)&left_child, 1, 4, f);
    fread((char*)&right_child, 1, 4, f);
    fread((char*)&count, 1, 4, f);
    fread((char*)&price, 1, 4, f);

    int ct = 0;
    char c = 0;
    for (;;) {
        ct += fread(&c, 1, 1, f);
        if (c == 0) {
            break;
        }
    }

    char word[ct];
    fseek(f, -ct, SEEK_CUR);
    fread(word, 1, ct, f);

    if (strcmp(search_word, word) == 0) {
        printFound(word, count, price);
        return;
    } else if (strcmp(search_word, word) < 0) {
        searchTree(f, left_child, search_word);
    } else {
        searchTree(f, right_child, search_word);
    }
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        printArgumentUsage();
        exit(1);
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL) {
        openFail(argv[1]);
        exit(2);
    }

    char checksum[5];
    memset(checksum, 0, 5);
    fread(checksum, 1, 4, f);
    if (strcmp(checksum, "BTRE")) {
        formatFail(argv[1]);
        fclose(f);
        exit(2);
    }

    for (int i = 2; i < argc; i++) {
        rewind(f);
        searchTree(f, 4, argv[i]);
    }

    fclose(f);
    return 0;
}
