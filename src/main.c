#include <stdio.h>
#include <stdlib.h>

#include "lpfile.h"

int main(int argc, char const *argv[]) {
    int bool;

    if(argc < 2) {
        printf("Input file not found!\n");
        return 1;
    }

    const char *path  = argv[1];

    bool = lpp_load(path);
    if(bool){
        switch (bool)
        {
        case 1:
            printf("Input file not found!\n");
            return 1;
            break;

        case 11:
            printf("Syntax Error\n");
            return 11;
            break;
        
        default:
            break;
        }
    }
    
    return EXIT_SUCCESS;
}