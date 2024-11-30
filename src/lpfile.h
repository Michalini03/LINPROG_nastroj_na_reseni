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
    double constraints[MAX_ROWS][MAX_VARS];
    double rhs[MAX_VARS];

    double lower_bounds[MAX_VARS];
    double upper_bounds[MAX_VARS];
};

/* Načte soubor LP a naplní strukturu LPProblem */
int lpp_load(const char* path, struct LPProblem **lpp);

/* TODO: Funkce, která zapíše výsledky LP do zdrojového souboru */
int lpp_write();

/* TODO: Funkce, která vyřeší zadaný úkol lineárního programování */
void lpp_solve(struct LPProblem *lp);

/* Funkce pro alokaci a inicializaci LPProblem */
struct LPProblem* lpp_alloc(double objective[], double constraints[][MAX_VARS], double rhs[], int num_vars, int num_constraints, double lower_bounds[], double upper_bound[]);

/* Funkce pro inicializaci struktury LPProblem */
int lpp_init(struct LPProblem *lp, double objective[], double constraints[][MAX_VARS], double rhs[], int num_vars, int num_constraints, double lower_bounds[], double upper_bound[]);

/* TODO: Funkce pro uvolňění paměti */
int lpp_dealloc(struct LPProblem **lp);

/* TODO: Funkce pro odebrání atributů ze struktury LPProblem */
void lpp_deinit(struct LPProblem *lp);

/* Připravení řádku zdrojové dokumentu pro další úpravy */
void lpp_prepare_str(char *str);

/* 
Každý řádek podsekce by měl začínat odsazením, tato funkce provede 
kontrolu, jestli se tak skutečně děje.
*/
int lpp_undersection_control(const char* str);

/* Jestliže, je řádek z podsekce, připraví ho tím, že přeskočí odsazení */
void lpp_undersection_prepare(char *line);

#endif