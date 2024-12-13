#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lpfile.h"

#define INPUT_FILE_CMD_1 "-o"
#define INPUT_FILE_CMD_2 "--output"

int main(int argc, char const *argv[]) {
    int bool, result;
    const char *path;
    struct LPProblem *LPPSolver;

    /* Pokud uživatel nezadá argument pro cestu k input souboru, program rovnou selže */
    if(argc < 2) {
        printf("Input file not found!\n");
        return 1;
    }

    path  = argv[1];

    bool = lpp_load(path, &LPPSolver);

    if(!LPPSolver) {
        printf("Allocation failed.\n");
        return 3;
    }

    /* Ověření, jestli se funkce lpp_load povedla */
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
        
        case 3:
            printf("Allocation failed.\n");
            return 3;
            break;
        
        default:
            break;
        }
    }

    result = lpp_solve(LPPSolver);
    
    if(result) {
        switch (result)
        {
        case 20:
            printf("Objective function is unbounded.\n");
            return 20;
            break;
        
        case 21:
            printf("No feasible solution exists.\n" );
            return 21;
            break;
        
        case 3:
            printf("Allocation failed.\n");
            return 3;
            break;
        
        default:
            break;
        }
    }

    /* Kontrola pro správné zadání output souboru */
    if(argc > 2) {
        if(argc == 3 || argc > 4) {
            printf("Invalid output destination!\n");
            return 2;
        }
        if(strcmp(argv[2], INPUT_FILE_CMD_1) != 0 && strcmp(argv[2], INPUT_FILE_CMD_2) != 0) {
            printf("Invalid output destination!\n");
            return 2;
        }
        if(lpp_write(LPPSolver, argv[3])) {

        }
    }
    else {

    }
    return EXIT_SUCCESS;
}