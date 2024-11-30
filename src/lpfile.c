#include <stdio.h>
#include <string.h>
#include "lpfile.h"
#include <ctype.h>

#define NUM_OF_SECTORS 5

#define NUM_OF_SUBJECT_TO 1
#define NUM_OF_MAX 2
#define NUM_OF_BOUNDS 3
#define NUM_OF_GENERALS 4

#define LINE_LENGHT 256


int lpp_load(const char* path, struct LPProblem **lpp) {

    /* Proměnné, které se budou naplňovat pro inicializaci struktury LPProblem */
    double objective[MAX_VARS];
    double constraints[MAX_ROWS][MAX_VARS];
    double rhs[MAX_VARS];
    int num_vars = 0; 
    int num_constraints = 0;

    double lower_bounds[MAX_VARS];
    double upper_bounds[MAX_VARS];

    /* Proměnné pro kontrolu použitých proměnných  */
    int num_vars_in_generals = 0;
    char generals[MAX_VARS][LINE_LENGHT]; /* Pole pro názvy proměnných ze sekce GENERALS */
    char used_generals[MAX_VARS][LINE_LENGHT]; /* Pole pro názvy proměnných využitých v účelové funkci */

    /* Proměnné pro ověření správného počtu sektorů a pro označení aktuálně procházeného sektoru */
    int num_of_sector = 0;
    int count_of_sectors = 0;

    FILE *file = fopen(path, "r");
    if (!file) {
        printf("Input file not found!\n");
        return 1;
    }

    char line[LINE_LENGHT];
    while (fgets(line, sizeof(line), file) != NULL) {

        /* Odstraní mezery na začátku a konci */
        lpp_prepare_str(line);

        /* Pokud je řádek prázdný po oříznutí, pokračujeme */
        if (line[0] == '\0') {
            continue;
        }

        if (num_of_sector) {
            switch (num_of_sector) {
                case NUM_OF_SUBJECT_TO:
                    do {
                        lpp_prepare_str(line);
                        if (!lpp_undersection_control(line)) {
                            num_of_sector = 0;
                            break;
                        }
                        else
                        {
                            lpp_undersection_prepare(line);
                        }

                        /* TODO: Zpracování řádku sekce "Subject To" */

                    } while (fgets(line, sizeof(line), file) != NULL);
                    break;
                    
               case NUM_OF_MAX:
                    do {
                        lpp_prepare_str(line);
                        if (!lpp_undersection_control(line)) {
                            num_of_sector = 0;
                            break;
                        } else {
                            lpp_undersection_prepare(line);
                        }

                        /* Rozdělení řádku na jednotlivé termy podle '+' a '-' */
                        char *term = strtok(line, "+-");
                        int sign = 1; /* Výchozí znaménko je kladné */

                        while (term != NULL) {
                            double coefficient = 1.0;
                            char variable[LINE_LENGHT];

                            /* Pokud je před termem '-', změníme znaménko */
                            if (term > line && *(term - 1) == '-') {
                                sign = -1;
                            } else {
                                sign = 1;
                            }

                            /* Zpracování termu */
                            /* Oprava pro termy typu '2 * 2x_2' nebo '3x_1' */
                            char *mul_pos = strchr(term, '*'); /* Zjištění, jestli je v termu '*' */
                            double left_coeff = 1 * sign;
                            double used_coeff;
                            while (mul_pos) {
                                /* Pokud je '*' v termu, jedná se o součin */                                
                                char right_variable[LINE_LENGHT];
                                
                                /* Parsujeme levý koeficient a proměnnou s operátorem '*' */
                                if (sscanf(term, "%lf * %s", &used_coeff, right_variable) == 2) {
                                    left_coeff = left_coeff * used_coeff;
                                    strcpy(term, right_variable);
                                } else {
                                    printf("Syntax error in Maximize/Minimize section!\n");
                                    fclose(file);
                                    goto syntax_error;
                                }
                                mul_pos = strchr(term, '*');
                            }
                            
                                /* Pokud není '*' ve výrazu, jedná se o obyčejný term */
                            if (sscanf(term, "%lf%s", &coefficient, variable) == 2) {
                                coefficient = coefficient * left_coeff;
                            } else if (sscanf(term, "%s", variable) == 1) {
                                coefficient = 1.0 * sign; /* Implicitní koeficient je 1 */
                            } else {
                                printf("Syntax error in Maximize/Minimize section!\n");
                                fclose(file);
                                goto syntax_error;
                            }

                            /* Kontrola, zda není pole přeplněné */
                            if (num_vars >= MAX_VARS) {
                                printf("Error: Too many variables in objective function!\n");
                                fclose(file);
                                goto syntax_error;
                            }

                            /* Uložení koeficientu a proměnné */
                            objective[num_vars] = coefficient;
                            strcpy(used_generals[num_vars], variable);
                            num_vars++;

                            /* Další term */
                            term = strtok(NULL, "+-");
                        }
                    } while (fgets(line, sizeof(line), file) != NULL);
                    break;


                case NUM_OF_BOUNDS:
                    do {
                        lpp_prepare_str(line);
                        if (!lpp_undersection_control(line)) {
                            num_of_sector = 0;
                            break;
                        }
                        else
                        {
                            lpp_undersection_prepare(line);
                        }

                        /* TODO: Zpracování řádku sekce "Bounds" */

                    } while (fgets(line, sizeof(line), file) != NULL);
                    break;

                case NUM_OF_GENERALS:
                    do {
                        lpp_prepare_str(line); /* Odstranění mezer a komentářů */
                        if (!lpp_undersection_control(line)) {
                            num_of_sector = 0;
                            break;
                        } else {
                            lpp_undersection_prepare(line); /* Odstranění tabulátorů */
                        }

                        char *token = strtok(line, " "); /* Rozdělení řádku na části podle mezer */
                        while (token != NULL) {
                            /* Kontrola, zda není pole přeplněné */
                            if (num_vars_in_generals >= MAX_VARS) {
                                printf("Error: Too many variables in Generals section!\n");
                                fclose(file);
                                goto syntax_error;
                            }

                            /* Uložení názvu proměnné */
                            strcpy(generals[num_vars_in_generals++], token);

                            /* Další token */
                            token = strtok(NULL, " ");
                        }
                    } while (fgets(line, sizeof(line), file) != NULL);
                    break;

                default:
                    goto syntax_error;
            }
        }
        if (strcmp(line, "Subject To") == 0) {
            count_of_sectors += 1;
            num_of_sector = NUM_OF_SUBJECT_TO;
        } 
        else if (strcmp(line, "Maximize") == 0 || strcmp(line, "Minimize") == 0) {
            count_of_sectors += 1;
            num_of_sector = NUM_OF_MAX;
        } 
        else if (strcmp(line, "Generals") == 0) {
            count_of_sectors += 1;
            num_of_sector = NUM_OF_GENERALS;
        } 
        else if (strcmp(line, "Bounds") == 0) {
                count_of_sectors += 1;
                num_of_sector = NUM_OF_BOUNDS;
        } 
        else if (strcmp(line, "End") == 0) {
            count_of_sectors += 1;
             break;
        } 
        else {
            goto syntax_error;
        }    
    }

    fclose(file);

    if (count_of_sectors != NUM_OF_SECTORS) {
        syntax_error: return 11;
    }

    int is_there, i, j;
    for(i = 0; i < num_vars_in_generals; i++) {
        is_there = 0;
        for(j = 0; j < num_vars; j++) {
            if(strcmp(generals[i], generals[j]) == 0) {
                is_there = 1;
            }
        }
        if(!is_there) {
            printf( "Warning: unused variable %s!\n", generals[i]);
        }
    }

    *lpp = lpp_alloc(objective, constraints, rhs, num_vars, num_constraints, lower_bounds, upper_bounds);
    if(!*lpp) {
        /* return 11; */
        printf("TODO - INIT\n");
    }

    return 0;
}


struct LPProblem* lpp_alloc(double objective[], double constraints[][MAX_VARS], double rhs[], int num_vars, int num_constraints, double lower_bounds[], double upper_bounds[]) {
    struct LPProblem *new_lpp = malloc(sizeof(struct LPProblem));
    if (!new_lpp) {
        return NULL;
    }
    if(!lpp_init(new_lpp, objective, constraints, rhs, num_vars, num_constraints, lower_bounds, upper_bounds)) {
        return NULL;
    }
    return new_lpp;
}


int lpp_init(struct LPProblem *lp, double objective[], double constraints[][MAX_VARS], double rhs[], int num_vars, int num_constraints, double lower_bounds[], double upper_bounds[]) {
    if (!lp || !objective || !constraints || !rhs || num_vars < 1 || num_constraints < 0) {
        return 0;
    }

    lp->num_vars = num_vars;
    lp->num_constraints = num_constraints;

    int i;
    int j;

    for (i = 0; i < num_vars; i++) {
        lp->objective[i] = objective[i];
    }

    for (i = 0; i < num_constraints; i++) {
        for (j = 0; j < num_vars; j++) {
            lp->constraints[i][j] = constraints[i][j];
        }
    }

    for (i = 0; i < num_constraints; i++) {
        lp->rhs[i] = rhs[i];
    }

    return 1;
}


void lpp_prepare_str(char *str) {
    char *end;

    /* Odstraní znak nového řádku */
    str[strcspn(str, "\n")] = '\0';

    /* Najde pozici prvního výskytu '\' a ukončí řetězec na této pozici (ignorování komentářů) */
    char *comment_pos = strchr(str, '\\');
    if (comment_pos) {
        *comment_pos = '\0';
    }

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) {
        return;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';
}


int lpp_undersection_control(const char* str) {
    return (strncmp(str, "\t", 1) == 0);
}


void lpp_undersection_prepare(char *line) {
    /* Najde první znak, který není tabulátor */
    char *start = line;
    while (*start == '\t') {
        start++;
    }

    /* Přesune řetězec na začátek */
    if (start != line) {
        memmove(line, start, strlen(start) + 1);
    }
}
