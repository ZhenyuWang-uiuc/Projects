/**
 * mad_mad_access_patterns
 * CS 241 - Spring 2022
 */
#include "tree.h"
#include "utils.h"
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/

void searchTree(char* ptr, uint32_t offset, char* search_word) {
    if (offset == 0) {
        printNotFound(search_word);
        return;
    }
        
    // locate the node
    char* temp = ptr + offset;

    uint32_t left_child = 0;
    uint32_t right_child = 0;
    uint32_t count = 0;
    float price = 0;

    for (int i = 0; i < 4; i++) {
        ((char*)&left_child)[i] = temp[i];
        ((char*)&right_child)[i] = temp[i + 4];
        ((char*)&count)[i] = temp[i + 8];
        ((char*)&price)[i] = temp[i + 12];
    }

    char word[strlen(temp + 16) + 1];
    strcpy(word, temp + 16);

    if (strcmp(search_word, word) == 0) {
        printFound(word, count, price);
        return;
    } else if (strcmp(search_word, word) < 0) {
        searchTree(ptr, left_child, search_word);
    } else {
        searchTree(ptr, right_child, search_word);
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

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    rewind(f);

    char* ptr = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fileno(f), 0);
    if (ptr == MAP_FAILED) {
        mmapFail(argv[1]);
        exit(3);
    }
    
    char c = ptr[4];
    *(ptr + 4) = 0;
    if (strcmp(ptr, "BTRE")) {
        formatFail(argv[1]);
        fclose(f);
        exit(2);
    }
    *(ptr + 4) = c;

    for (int i = 2; i < argc; i++) {
        searchTree(ptr, 4, argv[i]);
    }

    fclose(f);
    return 0;
}
