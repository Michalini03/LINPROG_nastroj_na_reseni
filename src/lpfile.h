#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LPFILE_H
#define LPFILE_H

#define MAX_VARS 100       /* Maximální počet proměnných */
#define MAX_ROWS 100       /* Maximální počet omezení */

struct LPProblem {
    int num_vars;
    int num_constraints;
    double objective[MAX_VARS];
    double constraints[MAX_VARS][MAX_VARS];
    double rhs[MAX_VARS];
};

/* Načte soubor LP a naplní strukturu LPProblem */
int lpp_load(const char* path);

/* Funkce pro alokaci a inicializaci LPProblem */
struct LPProblem* lpp_alloc(double objective[], double constraints[][MAX_VARS], double rhs[], int num_vars, int num_constraints);

/* Funkce pro inicializaci struktury LPProblem */
int lpp_init(struct LPProblem *lp, double objective[], double constraints[][MAX_VARS], double rhs[], int num_vars, int num_constraints);

#endif