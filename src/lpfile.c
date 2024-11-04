#include <stdio.h>

#include "lpfile.h"


int load_input_lpfile(char const* path, struct lp **body) {
    if(!test_input_lpfile(path)) {
        return 0;
    }
    FILE *fptr;
    fptr = fopen(path, "rb+");
    return 0;
}

int test_input_lpfile(char const* path) {
    FILE *fptr;
    fptr = fopen(path, "rb+");
    if(!fptr) {
        printf("Input file not found\n");
        fclose(fptr);
        return 0;
    }
    return 1;
}