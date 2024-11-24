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

/* Funkce pro odstranění mezer na začátku a konci řetězce */
void trim(char *str) {
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

/* Funkce pro kontrolu začátku řádku tabulátorem */
int starts_with_tab(const char* str) {
    return (strncmp(str, "\t", 1) == 0);
}

/* Funkce pro odstranění tabulátoru ze začátku řádku */
void remove_leading_tabs(char *line) {
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

int lpp_load(const char* path, struct LPProblem **lpp) {

    /* Promměné, které se budou naplňovat pro inicializaci struktury LPProblem */
    double objective[MAX_VARS];
    double constraints[MAX_ROWS][MAX_VARS];
    double rhs[MAX_VARS];
    int num_vars = 0; 
    int num_constraints = 0;


    int num_vars_in_generals = 0;
    char generals[MAX_VARS][LINE_LENGHT]; /* Pole pro názvy proměnných */

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
        trim(line);

        /* Pokud je řádek prázdný po oříznutí, pokračujeme */
        if (line[0] == '\0') {
            continue;
        }

        if (num_of_sector) {
            switch (num_of_sector) {
                case NUM_OF_SUBJECT_TO:
                    do {
                        trim(line);
                        if (!starts_with_tab(line)) {
                            num_of_sector = 0;
                            break;
                        }
                        else
                        {
                            remove_leading_tabs(line);
                        }

                        /* TODO: Zpracování řádku sekce "Subject To" */

                    } while (fgets(line, sizeof(line), file) != NULL);
                    break;
                    
                case NUM_OF_MAX:
                    do {
                        trim(line);
                        if (!starts_with_tab(line)) {
                            num_of_sector = 0;
                            break;
                        }
                        else
                        {
                            remove_leading_tabs(line);
                        }

                        /* TODO: Zpracování řádku sekce "Maximize / Minimize" */

                    } while (fgets(line, sizeof(line), file) != NULL);
                    break;

                case NUM_OF_BOUNDS:
                    do {
                        trim(line);
                        if (!starts_with_tab(line)) {
                            num_of_sector = 0;
                            break;
                        }
                        else
                        {
                            remove_leading_tabs(line);
                        }

                        /* TODO: Zpracování řádku sekce "Bounds" */

                    } while (fgets(line, sizeof(line), file) != NULL);
                    break;

                case NUM_OF_GENERALS:
                    do {
                        trim(line); /* Odstranění mezer a komentářů */
                        if (!starts_with_tab(line)) {
                            num_of_sector = 0;
                            break;
                        } else {
                            remove_leading_tabs(line); /* Odstranění tabulátorů */
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

    *lpp = lpp_alloc(objective, constraints, rhs, num_vars, num_constraints);
    if(!*lpp) {
        /* return 11; */
        printf("TODO - INIT\n");
    }

    return 0;
}

/* Funkce pro alokaci LPProblem */
struct LPProblem* lpp_alloc(double objective[], double constraints[][MAX_VARS], double rhs[], int num_vars, int num_constraints) {
    struct LPProblem *new_lpp = malloc(sizeof(struct LPProblem));
    if (!new_lpp) {
        return NULL;
    }
    if(!lpp_init(new_lpp, objective, constraints, rhs, num_vars, num_constraints)) {
        return NULL;
    }
    return new_lpp;
}

/* Funkce pro inicializaci LPProblem */
int lpp_init(struct LPProblem *lp, double objective[], double constraints[][MAX_VARS], double rhs[], int num_vars, int num_constraints) {
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
