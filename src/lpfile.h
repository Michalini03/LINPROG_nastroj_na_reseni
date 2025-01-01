#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplex.h"
#include "shunt.h"

#ifndef LPFILE_H
#define LPFILE_H

#define MAX_VARS 10       /* Maximální počet proměnných */
#define MAX_ROWS 10       /* Maximální počet omezení */

#define LINE_LENGHT 256   /* Nadefinovaná maximální délka řádku */

/* Ověření správnosti procházení sektorů */
#define NUM_OF_SECTORS 5

/* Označení pro sektory */
#define NUM_OF_SUBJECT_TO 1
#define NUM_OF_MAX 2
#define NUM_OF_BOUNDS 3
#define NUM_OF_GENERALS 4


/**
 * \struct LPProblem
 * \brief Struktura problému lineárního programování (LP).
 */
struct LPProblem {
    int num_vars;                               /**< Počet proměnných. */
    int num_constraints;                        /**< Počet omezení. */

    double objective[MAX_VARS];                 /**< Koeficienty účelové funkce. */
    double constraints[MAX_ROWS][MAX_VARS];     /**< Koeficienty omezovacích funkcí. */
    double rhs[MAX_VARS];                       /**< Pravé strany omezovacích funkcí. */

    double lower_bounds[MAX_VARS];              /**< Spodní hranice hodnot. */
    double upper_bounds[MAX_VARS];              /**< Horní hranice hodnot. */

    char vars[MAX_VARS][LINE_LENGTH];           /**< Jména použitých proměnných. */
    char b_column_vars[MAX_ROWS][LINE_LENGTH];  /**< Jména proměnných v bázovém sloupci. */
    char operators[MAX_VARS][LINE_LENGTH];      /**< Použíte operátory omezovacích funkcí. */
};

/**
 * \brief lpp_load Funkce vyčte potřebná data z inputu 
 *      a pomocí funkcí 'lpp_alloc' a 'lpp_init' naplní pointer daty.                     
 * \param path Adresa na input soubor.
 * \param lpp Ukazatel na ukazatel na strukturu 'LPProblem'
 * \return Číselná hodnota východu z funkce, pokud se vrátí nula, vše je v pořádku.
 */
int lpp_load(const char* path, struct LPProblem **lpp);

/**
 * \brief lpp_write Funkce vypíše výsledek do ouptutu.
 * \param lp Pointer na strukturu 'LPProblem', která má v sobě uložený výsledek.
 * \param output_path Adresa na output soubor.
 * \return Číselná hodnota východu z funkce, pokud se vrátí nula, vše je v pořádku.
 */
int lpp_write(struct LPProblem *lp, const char output_path[]);

/**
 * \brief lpp_print Funkce vypíše výsledek do konzole.
 * \param lp Pointer na strukturu 'LPProblem', která má v sobě uložený výsledek.
 * \return Číselná hodnota východu z funkce, pokud se vrátí nula, vše je v pořádku.
 */
int lpp_print(struct LPProblem *lp);

/**
 * \brief lpp_write Funkce vyřeší pomocí simplexova algoritmu zadaný problém
 * \param lp Pointer na strukturu 'LPProblem', který má v sobě data pro řešení úlohy.
 * \return Číselná hodnota východu z funkce, pokud se vrátí nula, vše je v pořádku.
 */
int lpp_solve(struct LPProblem *lp);

/**
 * \brief lpp_alloc Funkce alokuje místo na paměti pro strukturu 'LPProblem' a poté zavolá ještě 'lpp_init', 
 *          pro její inicializaci.
 * \param objective Koeficienty účelové funkce.
 * \param constraints Koeficienty všech omezovacích funkcí.
 * \param rhs Pravé strany omezovacích funkcí.
 * \param vars Jména všech použitých proměnných.
 * \param num_vars Počet proměnných.
 * \param num_constraints Počet omezení.
 * \param lower_bounds Spodní hranice proměnných.
 * \param upper_bounds Horní hranice proměnných.
 * \param operators Operátory omezovacích funkcí (<=, >=, =).
 * \return Vracíme pointer na nově inicializovaný 'LPProblem', v případě chyby NULL.
 */
struct LPProblem* lpp_alloc(double objective[], double constraints[][MAX_VARS], double rhs[], char vars[MAX_VARS][LINE_LENGHT], int num_vars, int num_constraints, double lower_bounds[], double upper_bound[], char operators[MAX_VARS][LINE_LENGTH]);

/**
 * \brief lpp_init Funkce inicializuje strukturu 'LPProblem'.
 * \param lp Pointer na strukturu 'LPProblem', do kterého uložíme hodnoty.
 * \param objective Koeficienty účelové funkce.
 * \param constraints Koeficienty všech omezovacích funkcí.
 * \param rhs Pravé strany omezovacích funkcí.
 * \param vars Jména všech použitých proměnných.
 * \param num_vars Počet proměnných.
 * \param num_constraints Počet omezení.
 * \param lower_bounds Spodní hranice proměnných.
 * \param upper_bounds Horní hranice proměnných.
 * \param operators Operátory omezovacích funkcí (<=, >=, =).
 * \return Vracíme číselnou hodnotu, v případě bezchybnosti se vrátí jednička.
 */
int lpp_init(struct LPProblem *lp, double objective[], double constraints[][MAX_VARS], double rhs[], char vars[MAX_VARS][LINE_LENGHT], int num_vars, int num_constraints, double lower_bounds[], double upper_bound[], char operators[MAX_VARS][LINE_LENGTH]);

/**
 * \brief lpp_dealoc Funkce nejdříve zavolá 'lpp_deinit' a poté nad danou alokovanou pamětí struktury
 *          'LPProblem' zavolá funkci 'free'.
 * \param lp Pointer na strukturu 'LPProblem', která má v uložené hodnoty.
 * \return Vracíme void pointer.
 */
int lpp_dealloc(struct LPProblem **lp);

/**
 * \brief lpp_deinit Funkce deinicializuje strukturu 'LPProblem'.
 * \param lp Pointer na strukturu 'LPProblem', která má v uložené hodnoty.
 * \return Vracíme void pointer.
 */
void lpp_deinit(struct LPProblem *lp);

/**
 * \brief lpp_prepare_str Funkce se zbaví zbytečných mezer, odstraní tabulátory a smaže komentáře, tím
 *          připraví řetězec pro další použítí
 * \param str Řetězec pro úpravu.
 * \return Vracíme void pointer.
 */
void lpp_prepare_str(char *str);

/**
 * \brief lpp_check_line Funkce zkontroluje řádek od inputu uživatele.
 * \param line Řetězec řádku z input souboru.
 * \return Číselná hodnota východu z funkce, pokud se vrátí nula, vše je v pořádku.
 */
int lpp_check_line(const char *line);

#endif