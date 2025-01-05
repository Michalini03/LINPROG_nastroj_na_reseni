#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lpfile.h"

/** 1. Definice příkazů pro vložení output souboru. */
#define INPUT_FILE_CMD_1 "-o"
/** 2. Definice příkazů pro vložení output souboru. */
#define INPUT_FILE_CMD_2 "--output"

int main(int argc, char const *argv[]) {
    int error, result;
    const char *input_path, *output_path;
    struct LPProblem *LPPSolver;
    int i;

    /* Pokud uživatel nezadá argument pro cestu k input souboru, program rovnou selže */
    if (argc < 2) {
        printf("Input file not found!\n");
        return 1;
    }

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], INPUT_FILE_CMD_1) == 0 || strcmp(argv[i], INPUT_FILE_CMD_2) == 0) {
            output_path = argv[i+1];
        }
        else if (strcmp(argv[i-1], INPUT_FILE_CMD_1) != 0 && strcmp(argv[i-1], INPUT_FILE_CMD_2) != 0 && strcmp(argv[i], INPUT_FILE_CMD_1) != 0 && strcmp(argv[i], INPUT_FILE_CMD_2) != 0) {
            input_path = argv[i];
        }
    }

    if (!input_path) {
        printf("Input file not found!\n");
        return 1;
    }

    error = lpp_load(input_path, &LPPSolver);

    /* Ověření, jestli se funkce lpp_load povedla */
    if (error){
        switch (error)
        {
        case 1:
            printf("Input file not found!\n");
            return 1;
            break;

        case 11:
            printf("Syntax error!\n");
            return 11;
            break;

        case 10:
            /* Zde se chybná hláška vypíše už ve funkci 'lpp_load' */
            return 10;
            break;
        
        case 3:
            printf("Allocation failed!.\n");
            return 3;
            break;
        
        default:
            break;
        }
    }

    if (!LPPSolver) {
        printf("Allocation failed.\n");
        return 3;
    }

    result = lpp_solve(LPPSolver);
    
    /* Ověření, jak dopadlo hledání výsledku */
    if (result) {
        switch (result) {
            case 20:
                printf("Objective function is unbounded.\n");
                lpp_dealloc(&LPPSolver);
                return 20;
                break;
            
            case 21:
                printf("No feasible solution exists.\n" );
                lpp_dealloc(&LPPSolver);
                return 21;
                break;
            
            case 3:
                printf("Allocation failed.\n");
                lpp_dealloc(&LPPSolver);
                return 3;
                break;
            
            default:
                break;
        }
    }

    /* Kontrola pro správné zadání output souboru */
    if (argc > 2) {
        error = lpp_write(LPPSolver, output_path);
        if(error) {
            switch (error)
            {
            case 2:
                printf("Invalid output destination!\n" );
                lpp_dealloc(&LPPSolver);
                return 2;
                break;
            
            case 3:
                printf("Allocation failed!\n" );
                lpp_dealloc(&LPPSolver);
                return 3;
                break;

            default:
                break;
            }
        }
    }
    else {
        lpp_print(LPPSolver);
    }

    lpp_dealloc(&LPPSolver);
    return EXIT_SUCCESS;
}