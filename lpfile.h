#include <stdio.h>

struct lp;

int load_input_lpfile(char const* path, struct lp **body) {
    FILE *fptr;
    fptr = fopen(path, "rb+");
    if(!fptr) {
        printf("Input file not found\n");
        return 1;
    }
    return 0;
};