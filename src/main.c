#include <stdio.h>
#include <stdlib.h>

#include "lpfile.h"

int main(int argc, char const *qrgv[]) {
    
    struct lp *problem;
    printf("%s\n", qrgv[1]);

    if(load_input_lpfile(qrgv[1], &problem)){
        return 1;
    }
    
    return EXIT_SUCCESS;
}