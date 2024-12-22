#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplex.h"
#include "shunt.h"

#ifndef LPFILE_H
#define LPFILE_H

#define MAX_VARS 100       /* Maximální počet proměnných */
#define MAX_ROWS 100       /* Maximální počet omezení */

#define LINE_LENGHT 256

struct LPProblem {
    int num_vars;
    int num_constraints;

    double objective[MAX_VARS];
    double constraints[MAX_ROWS][MAX_VARS];
    double rhs[MAX_VARS];

    double lower_bounds[MAX_VARS];
    double upper_bounds[MAX_VARS];

    char vars[MAX_VARS][LINE_LENGTH];
    char b_column_vars[MAX_ROWS][LINE_LENGTH];
    char operators[MAX_VARS][LINE_LENGTH];
};

/* Načte soubor LP a naplní strukturu LPProblem */
int lpp_load(const char* path, struct LPProblem **lpp);

/* TODO: Funkce, která zapíše výsledky LP do zdrojového souboru */
int lpp_write(struct LPProblem *lp, const char output_path[]);

int lpp_print(struct LPProblem *lp);

/* Funkce vyřeší LP nebo zjistí jeho neřešitelnost, hodnoty ukládá do atributu rh */
int lpp_solve(struct LPProblem *lp);

/* Funkce pro alokaci a inicializaci LPProblem */
struct LPProblem* lpp_alloc(double objective[], double constraints[][MAX_VARS], double rhs[], char vars[MAX_VARS][LINE_LENGHT], int num_vars, int num_constraints, double lower_bounds[], double upper_bound[], char operators[MAX_VARS][LINE_LENGTH]);

/* Funkce pro inicializaci struktury LPProblem */
int lpp_init(struct LPProblem *lp, double objective[], double constraints[][MAX_VARS], double rhs[], char vars[MAX_VARS][LINE_LENGHT], int num_vars, int num_constraints, double lower_bounds[], double upper_bound[], char operators[MAX_VARS][LINE_LENGTH]);

/* TODO: Funkce pro uvolňění paměti */
int lpp_dealloc(struct LPProblem **lp);

/* TODO: Funkce pro odebrání atributů ze struktury LPProblem */
void lpp_deinit(struct LPProblem *lp);

/* Připravení řádku zdrojové dokumentu pro další úpravy */
void lpp_prepare_str(char *str);

#endif