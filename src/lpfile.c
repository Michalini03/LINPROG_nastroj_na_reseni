#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lpfile.h"
#include <ctype.h>

int lpp_load(const char* path, struct LPProblem **lpp) {

    /* Sem budu načítat řádky */
    char line[LINE_LENGHT];
    char *token;
    char *colon;


    /* Proměnné, které se budou naplňovat pro inicializaci struktury LPProblem */
    double objective[MAX_VARS];
    int num_vars = 0; 

    char vars_order[MAX_ROWS][MAX_VARS][LINE_LENGHT];
    double prep_constraints[MAX_ROWS][MAX_VARS];
    double constraints[MAX_ROWS][MAX_VARS];


    double rhs[MAX_VARS];
    char operators[MAX_VARS][LINE_LENGTH];
    int num_constraints = 0;
    int bounds = 0;

    double lower_bounds[MAX_VARS];
    double upper_bounds[MAX_VARS];
    double lower_value, upper_value;
    char var[10];

    /* Proměnné pro kontrolu použitých proměnných  */
    int num_vars_in_generals = 0;
    char generals[MAX_VARS][LINE_LENGHT]; /* Pole pro názvy proměnných ze sekce GENERALS */
    char used_generals[MAX_VARS][LINE_LENGHT]; /* Pole pro názvy proměnných využitých v účelové funkci */

    /* Proměnné pro ověření správného počtu sektorů a pro označení aktuálně procházeného sektoru */
    int num_of_sector = 0;
    int count_of_sectors = 0;

    int is_there, i, j, k;
    int minimize = 0;

    /* Uložení postfixu */
    char **postfix;
    Expression result;

    /* Objekt pro procházení souborem */
    FILE *file;

    file = fopen(path, "r");
    if (!file) {
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {

        /* Odstraní mezery na začátku a konci */
        lpp_prepare_str(line);

        /* Pokud je řádek prázdný po oříznutí, pokračujeme */
        if (line[0] == '\0') {
            continue;
        }

        if (strcmp(line, "Subject To") == 0) {
            count_of_sectors += 1;
            num_of_sector = NUM_OF_SUBJECT_TO;
        } 
        else if (strcmp(line, "Maximize") == 0 || strcmp(line, "Minimize") == 0) {
            count_of_sectors += 1;
            num_of_sector = NUM_OF_MAX;
            if (strcmp(line, "Minimize") == 0) {
                minimize = 1;
            }
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
        else if (num_of_sector) {
            if(!lpp_check_line(line)) {
                fclose(file);
                return 11;
            }
            switch (num_of_sector) {
                case NUM_OF_MAX:
                    postfix = shunt_alloc_postfix();
                    if(!postfix) {
                        fclose(file);
                        return 3;
                    }
                    if(shunt_get_postfix(line, postfix)) {
                        shunt_free_postfix(postfix);
                        fclose(file);
                        return 11;
                    };
                    /* shunt_print_postfix(postfix); */
                    result = postfix_parse_expression(postfix);
                    if(result.size < 0) {
                        shunt_free_postfix(postfix);
                        postfix_free_expression(&result);
                        return 11;
                    }

                    /* printf("Result:\n"); */
                    /* postfix_print_expression(&result); */
                    num_vars = 0;
                    for (i = 0; i < result.size; i++) {
                        if(result.terms[i].variable) {
                            objective[num_vars] = result.terms[i].coefficient;
                            strcpy(used_generals[num_vars], result.terms[i].variable);
                            num_vars += 1;
                        }
                    }
                    postfix_free_expression(&result);
                    shunt_free_postfix(postfix);
                    postfix = NULL;
                    break;

                case NUM_OF_BOUNDS:
                    if(sscanf(line, "%lf <= %s <= %lf",&lower_value, var, &upper_value) == 3) {
                        lower_bounds[bounds] = lower_value;
                        upper_bounds[bounds] = upper_value;
                    }
                    else if(sscanf(line, "%lf >= %s >= %lf", &upper_value, var, &lower_value) == 3) {
                        lower_bounds[bounds] = lower_value;
                        upper_bounds[bounds] = upper_value;
                    }
                    else if(sscanf(line, "%s >= %lf", var, &lower_value) == 2) {
                        lower_bounds[bounds] = lower_value;
                        upper_bounds[bounds] = -1;
                    }
                    else if(sscanf(line, "%s <= %lf", var, &upper_value) == 2) {
                        lower_bounds[bounds] = -1;
                        upper_bounds[bounds] = upper_value;
                    }
                    else if(sscanf(line, "%lf <= %s", &lower_value, var) == 2) {
                        lower_bounds[bounds] = lower_value;
                        upper_bounds[bounds] = -1;
                    }
                    else {
                        fclose(file);
                        return 11;
                    }
                    bounds += 1;
                    lower_value = 0;
                    upper_value = 0;
                    break;

                case NUM_OF_SUBJECT_TO:
                    /* TODO: Zpracování řádku sekce "Subject To" */
                    if (strstr(line, "<=") != NULL) {
                        token = strtok(line, "<=");
                        strcpy(operators[num_constraints], "<="); 
                    }
                    else if (strstr(line, ">=") != NULL) {
                        token = strtok(line, ">=");
                        strcpy(operators[num_constraints], ">=");
                    }
                    else if (strstr(line, "=") != NULL) {
                        token = strtok(line, "=");
                        strcpy(operators[num_constraints], "=");
                    }
                    else {
                        fclose(file);
                        return 11;
                    }
                    colon = strchr(token, ':');

                    if (colon != NULL) {
                        token = colon + 1;
                        while (*token == ' ') {
                            token++;
                        }
                    }

                    lpp_prepare_str(token);
                    postfix = shunt_alloc_postfix();
                    if(!postfix) {
                        fclose(file);
                        return 3;
                    }
                    if(shunt_get_postfix(token, postfix)) {
                        shunt_free_postfix(postfix);
                        fclose(file);
                        return 11;
                    };

                    result = postfix_parse_expression(postfix);
                    num_vars = 0;
                    for (i = 0; i < result.size; i++) {
                        if(result.terms[i].variable) {
                            prep_constraints[num_constraints][num_vars] = result.terms[i].coefficient;
                            strcpy(vars_order[num_constraints][num_vars], result.terms[i].variable);
                            num_vars += 1;
                            }
                    }

                    postfix_free_expression(&result);
                    shunt_free_postfix(postfix);
                    postfix = NULL;

                    token = strtok(NULL, operators[num_constraints]);
                    lpp_prepare_str(token);
                    if (strstr(token, "<=") != NULL || strstr(token, ">=") != NULL || strstr(token, "=") != NULL) {
                        fclose(file);
                        return 11;
                    }
                    rhs[num_constraints] = strtod(token, NULL);
                    
                    num_constraints += 1;
                    break;
                    
                case NUM_OF_GENERALS:
                    token = strtok(line, " "); /* Rozdělení řádku na části podle mezer */
                    while (token != NULL) {
                        /* Kontrola, zda není pole přeplněné */
                        if (num_vars_in_generals >= MAX_VARS) {
                            fclose(file);
                            return 3;
                        }

                        /* Uložení názvu proměnné */
                        strcpy(generals[num_vars_in_generals++], token);

                        /* Další token */
                        token = strtok(NULL, " ");
                        
                    }
                    break;

                default:
                    fclose(file);
                    return 11;
                    break;
            }
        }
        /* Nepoznal jsem ani jednu sekci a ani žádnou neprocházím */
        else {
            fclose(file);
            return 11;
        }    
    }

    fclose(file);

    if (count_of_sectors != NUM_OF_SECTORS) {
        return 11;
    }

    /* Kontrola použitých proměnných */
    for(i = 0; i < num_vars_in_generals; i++) {
        is_there = 0;
        for(j = 0; j < num_vars; j++) {
            if(strcmp(generals[i], used_generals[j]) == 0) {
                is_there = 1;
            }
        }
        if(!is_there) {
            printf( "Warning: unused variable '%s'!\n", generals[i]);
        }
    }

    for(i = 0; i < num_vars; i++) {
        is_there = 0;
        for(j = 0; j < num_vars_in_generals; j++) {
            if(strcmp(used_generals[i], generals[j]) == 0) {
                is_there = 1;
            }
        }
        if(!is_there) {
            printf("Unknown variable '%s'!\n", used_generals[i]);
            return 10;
        }
    }

    /* Naformátování podle správného pořadí */
    for(i = 0; i < num_constraints; i++) {
        for(j = 0; j < num_vars; j++) {
            is_there = 0;
            for(k = 0; k < num_constraints; k++) {
                if(strcmp(used_generals[j], vars_order[i][k]) == 0) {
                    is_there = 1;
                    constraints[i][j] = prep_constraints[i][k];
                }
            }
            if(!is_there) {
                constraints[i][j] = 0;
            }
        }
    }


    /* Úprava hodnot v případě Minimize */
    if(minimize) {
        for(i = 0; i < num_vars; i++) {
            objective[i] = objective[i] * -1;
        }
        for(i = 0; i < num_constraints; i++) {
            if(rhs[i] < 0) {
                rhs[i] = rhs[i] * -1;
                for(j = 0; j < num_vars; j++) {
                    constraints[i][j] = constraints[i][j] * -1;
                }
                if(strcmp(operators[i], "<=") == 0) {
                    strcpy(operators[i], ">=");
                }
                else if(strcmp(operators[i], ">=") == 0) {
                    strcpy(operators[i], "<=");
                }
            }
        }
    }

    *lpp = lpp_alloc(objective, constraints, rhs, used_generals, num_vars, num_constraints, lower_bounds, upper_bounds, operators);
    if(!*lpp) {
        return 3;
    }

    return 0;
}


struct LPProblem* lpp_alloc(double objective[], double constraints[][MAX_VARS], double rhs[], char vars[MAX_VARS][LINE_LENGHT], int num_vars, int num_constraints, double lower_bounds[], double upper_bounds[], char operators[MAX_VARS][LINE_LENGTH]) {
    struct LPProblem *new_lpp = malloc(sizeof(struct LPProblem));
    if (!new_lpp) {
        return NULL;
    }
    if(!lpp_init(new_lpp, objective, constraints, rhs, vars, num_vars, num_constraints, lower_bounds, upper_bounds, operators)) {
        free(new_lpp);
        return NULL;
    }
    return new_lpp;
}


int lpp_init(struct LPProblem *lp, double objective[], double constraints[][MAX_VARS], double rhs[], char vars[MAX_VARS][LINE_LENGHT], int num_vars, int num_constraints, double lower_bounds[], double upper_bounds[], char operators[MAX_VARS][LINE_LENGTH]) {
    int i;
    int j;
    
    if (!lp || !objective || !constraints || !rhs || num_vars < 1 || num_constraints < 1) {
        return 0;
    }

    lp->num_vars = num_vars;
    lp->num_constraints = num_constraints;

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
        strcpy(lp->operators[i], operators[i]);
    }

    for(i = 0; i < num_vars; i++) {
        strcpy(lp->vars[i], vars[i]);
    }

    memcpy(lp->lower_bounds, lower_bounds, num_vars * sizeof(double));
    memcpy(lp->upper_bounds, upper_bounds, num_vars * sizeof(double));

    return 1;
}

int lpp_dealloc(struct LPProblem **lp) {
    if (!lp || !*lp) {
        return 0;
    }

    lpp_deinit(*lp);

    free(*lp);
    *lp = NULL;

    return 1;
}

void lpp_deinit(struct LPProblem *lp) {
    if (!lp) return;

    memset(lp->objective, 0, sizeof(lp->objective));
    memset(lp->constraints, 0, sizeof(lp->constraints));
    memset(lp->rhs, 0, sizeof(lp->rhs));
    memset(lp->lower_bounds, 0, sizeof(lp->lower_bounds));
    memset(lp->upper_bounds, 0, sizeof(lp->upper_bounds));
    memset(lp->vars, 0, sizeof(lp->vars));
    memset(lp->b_column_vars, 0, sizeof(lp->b_column_vars));
    memset(lp->operators, 0, sizeof(lp->operators));

    lp->num_vars = 0;
    lp->num_constraints = 0;
}

void lpp_prepare_str(char *str) {
    char *end, *comment_pos, *start;
    /* Odstraní znak nového řádku */
    str[strcspn(str, "\n")] = '\0';

    /* Odstraní odsazení */
    if(strncmp(str, "\t", 1) == 0) {
        start = str;
        while (*start == '\t') {
            start++;
        }

        /* Přesune řetězec na začátek */
        if (start != str) {
            memmove(str, start, strlen(start) + 1);
        }
    }

    /* Najde pozici prvního výskytu '\' a ukončí řetězec na této pozici (ignorování komentářů) */
    comment_pos = strchr(str, '\\');
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

int lpp_check_line(const char *line) {
    if(!line) {
        return 0;
    }
    if(strstr(line, "<=") != NULL) {
        if(strstr(line, ">=")) {
            return 0;
        }
    }
    if(strstr(line, "<==") != NULL || strstr(line, ">==") != NULL || strstr(line, "==") != NULL || strstr(line, "=>")  != NULL || strstr(line, "=<")  != NULL) { 
        return 0;
    }
    return 1;
}


int lpp_print(struct LPProblem *lp) {
    int i, j, found;
    if(lp == NULL) {
        return 0;
    }
    
    for(i = 0; i < lp->num_vars; i++) {
        found = 0;
        for(j = 0; j < lp->num_constraints; j++) {
            if(strcmp(lp->vars[i], lp->b_column_vars[j]) == 0) {
                printf("%s = %.4f\n", lp->vars[i], lp->rhs[j]);
                found = 1;
            }
        }
        if(!found) {
            printf("%s = 0\n", lp->vars[i]);
        }
    }
    return 1;
}

int lpp_write(struct LPProblem *lp, const char output_path[]) {
    FILE *fptr;
    int i, j, found;
    
    if (lp == NULL || output_path == NULL) {

    }

    fptr = fopen(output_path, "w");
    if (fptr == NULL) {
        return 2;
    }
    
    for(i = 0; i < lp->num_vars; i++) {
        found = 0;
        for(j = 0; j < lp->num_constraints; j++) {
            if(strcmp(lp->vars[i], lp->b_column_vars[j]) == 0) {
                fprintf(fptr, "%s = %.4f\n", lp->vars[i], lp->rhs[j]);
                found = 1;
            }
        }
        if(!found) {
            fprintf(fptr, "%s = 0\n", lp->vars[i]);
        }
    }
    fclose(fptr);
    return 0;
}

int lpp_solve(struct LPProblem *lp) {
    int i, j;

    double **simplex;

    /* Potřebné pole při práci se simplexovou maticí*/
    double *basis_column; /* Hodnoty bázových proměnných*/
    double *c_row;  /*Koeficienty rozšířené účelové funkce o slack a artifical variables */
    double *z_row;  /*Pole součtu součinů bázových proměnných a sloupců simplexovo matice*/
    double *c_z_row; /* c_row - z_row */

    /* Proměnné pro lepší orientaci */
    int num_vars, num_constraints;

    /* Souřadnice pivotního prvku */
    int pivot_column_index, pivot_row_index, reset_zero;
    double pivot_value;

    if(!lp) {
        return 3;
    }

    for(i = 0; i < lp->num_vars; i++) {
        if(lp->lower_bounds[i] != -1 && lp->upper_bounds[i] != -1) {
            if (lp->lower_bounds[i] > lp->upper_bounds[i]) {
                return 21;
            }
            
        }
    }
    
    num_vars = lp->num_vars;
    num_constraints = lp->num_constraints;

    /* Připrava simplexové matice */
    simplex = malloc(num_constraints * sizeof(double*));
    if(simplex == NULL) {
        return 3;
    }
    for (i = 0; i < num_constraints; ++i) {
        simplex[i] = malloc((num_vars + num_constraints * 2) * sizeof(double));
        if(simplex[i] == NULL) {
            return 3;
        }
    }

    for (i = 0; i < num_constraints; i++) {
        for (j = 0; j < num_vars; j++) {
            simplex[i][j] = lp->constraints[i][j];
        }
        if (strcmp(lp->operators[i],"<=") == 0) {
            simplex[i][i + num_vars] = 1;
        }
        else if (strcmp(lp->operators[i], ">=") == 0) {
            simplex[i][i + num_vars] = -1;
            simplex[i][i + num_vars + num_constraints] = 1;
        }
        else if (strcmp(lp->operators[i],"=") == 0) {
            simplex[i][i + num_vars + num_constraints] = 1;
        } 
    }

    c_row = simplex_prepare_c_row(num_vars, num_constraints, lp->objective, lp->operators);
    if (c_row == NULL) {
        for (i = 0; i < num_constraints; ++i) {
            free(simplex[i]);
        }
        free(simplex);
        return 3;
    }

    for (i = 0; i < num_constraints; i++) {
        if(strcmp(lp->operators[i], "<=") == 0) {
            strcpy(lp->b_column_vars[i], "s");
        }
        else {
            strcpy(lp->b_column_vars[i], "a");
        }
    }

    basis_column = simplex_prepare_basis_column(num_constraints, lp->operators);
    if (basis_column == NULL) {
        free(c_row);
        for (i = 0; i < num_constraints; ++i) {
            free(simplex[i]);
        }
        free(simplex);
        return 3;
    }
    
    z_row = simplex_prepare_z_row(num_vars, num_constraints, basis_column, simplex);
    if (z_row == NULL) {
        free(basis_column);
        free(c_row);
        for (i = 0; i < num_constraints; ++i) {
            free(simplex[i]);
        }
        free(simplex);
        return 3;
    }
    c_z_row = simplex_prepare_c_z_row(num_vars + (num_constraints * 2), c_row, z_row);
    if (c_z_row == NULL) {
        free(basis_column);
        free(c_row);
        free(z_row);
        for (i = 0; i < num_constraints; ++i) {
            free(simplex[i]);
        }
        free(simplex);
        return 3;
    }

    while (simplex_check_optimal_solution(c_z_row, num_vars + (num_constraints * 2))) {
        /* printf("\n BASIS  \n");
        for(i = 0; i < num_constraints; i++) {
            printf("%s | %f\n",lp->b_column_vars[i], basis_column[i]);
        }
        printf("\n C_ROW \n");
        for(i = 0; i < num_vars + num_constraints*2; i++) {
            printf("%f ", c_row[i]);
        }
        printf("\n Z_ROW \n");
        for(i = 0; i < num_vars + num_constraints*2; i++) {
        printf("%f ", z_row[i]);
        }    
        printf("\n C_Z_ROW \n");
        for(i = 0; i < num_vars + num_constraints*2; i++) {
            printf("%f ", c_z_row[i]);
        }
        printf("\n");
        printf("-----------------------------------------------------------\n"); */

        pivot_column_index = simplex_find_pivot(c_z_row, num_vars + (num_constraints * 2));
        if (pivot_column_index < 0 || pivot_column_index >= num_vars + (num_constraints * 2)) {
            free(c_row);
            free(basis_column);
            free(z_row);
            free(c_z_row);
            for (i = 0; i < num_constraints; ++i) {
                free(simplex[i]);
            }
            free(simplex);
            return 20;
        }

        pivot_row_index = simplex_find_basis_replace(lp->rhs, pivot_column_index, num_constraints, simplex);
        if (pivot_row_index < 0 || pivot_row_index >= num_constraints) {
            free(c_row);
            free(basis_column);
            free(z_row);
            free(c_z_row);
            for (i = 0; i < num_constraints; ++i) {
                free(simplex[i]);
            }
            free(simplex);
            return 20; /* Pokud jsem nenašel ani jedno kladné, funkce je neomezená */
        }

        /* V případě, že nehrazuji Artifical Variable, musím se jí zbavit */
        if (strcmp(lp->b_column_vars[pivot_row_index], "a") == 0 && pivot_column_index < num_vars + num_constraints) {
            
            reset_zero = pivot_row_index + num_vars + num_constraints;

            for (i = 0; i < num_constraints; i++) {
                simplex[i][reset_zero] = 0;
            }
            c_row[reset_zero] = 0;
            z_row[reset_zero] = 0;
            c_z_row[reset_zero] = 0;
        }
        basis_column[pivot_row_index] = c_row[pivot_column_index];

        pivot_value = simplex[pivot_row_index][pivot_column_index];
        if(pivot_value <= 0) {
            return 20;
        }

        /* Změná názvů proměnných */
        if(pivot_column_index >= num_vars) {
            if (pivot_column_index >= num_vars + num_constraints) {
                strcpy(lp->b_column_vars[pivot_row_index], "a");
            }
            else {
                strcpy(lp->b_column_vars[pivot_row_index], "s");
            }
        }
        else {
            strcpy(lp->b_column_vars[pivot_row_index], lp->vars[pivot_column_index]);
        }

        /* Úprava řádku pivotu */
        for (i = 0; i < num_vars + num_constraints; i++) {
            simplex[pivot_row_index][i] /= pivot_value;
        }
        lp->rhs[pivot_row_index] /= pivot_value;

        /* Úprava ostatních řádků */
        for (i = 0; i < num_constraints; i++) {
            if (i != pivot_row_index) {
                double main_col_val = simplex[i][pivot_column_index];
                for (j = 0; j < num_vars + num_constraints; j++) {
                    simplex[i][j] -= simplex[pivot_row_index][j] * main_col_val;
                }
                lp->rhs[i] -= lp->rhs[pivot_row_index] * main_col_val;
            }
        }

        free(z_row);
        free(c_z_row);
        z_row = NULL;
        c_z_row = NULL;
        z_row = simplex_prepare_z_row(num_vars, num_constraints, basis_column, simplex);
        c_z_row = simplex_prepare_c_z_row(num_vars + (num_constraints * 2), c_row, z_row);

    }

    free(c_row);
    free(basis_column);
    free(z_row);
    free(c_z_row);
    for (i = 0; i < num_constraints; ++i) {
        free(simplex[i]);
    }
    free(simplex);

    for(i = 0; i < num_constraints; i++) {
        if(strcmp(lp->b_column_vars[i], "a") == 0) {
            return 21;
        }
    }

    for(i = 0; i < lp->num_vars; i++) {
        for(j = 0; j < lp->num_constraints; j++) {
            if(strcmp(lp->vars[i], lp->b_column_vars[j]) == 0) {
                if((lp->lower_bounds[i] > lp->rhs[j] && lp->lower_bounds[i] != -1) || (lp->upper_bounds[i] < lp->rhs[j] && lp->upper_bounds[i] != -1)) {
                    return 21;
                }
            }
        }
    }


    return 0;
}