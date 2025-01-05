#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SIMPLEX_H
#define SIMPLEX_H

/** Maximální velikost řádku */
#define LINE_LENGTH 256
#define MAX_VARS 10       /* Maximální počet proměnných */
#define MAX_ROWS 10       /* Maximální počet omezení */

/** Definice M pro algoritmus Big M Tablue */
#define M 10000

/** Při operacích s doubly je možné, že se mi posune trochu nějaká desetinná hodnota,
 * toto zařizuje kontrolu.
 */
#define EPSILON 0.000000001

/**
 * \brief simplex_prepare_simplex_table Funkce připraví simplexovou tabulku pomocí koeficientů ze soustavy omezujících rovnic,
 *          a hodnot slack a artificial proměnných..
 * \param num_bounds Počet omezení.
 * \param num_vars Počet proměnných.
 * \param constraints Koeficienty soustavy rovnic omezujících funkcí.
 * \param operators Operátory omezujících funkcí.
 * \return Matice na doubly, reprecentující simplexovu tabulku.
 */
double **simplex_prepare_simplex_table(const int num_bounds, const int num_vars, double constraints[MAX_ROWS][MAX_VARS], char operators[][LINE_LENGTH]);

/**
 * \brief simplex_prepare_c_row Funkce připraví C řádek, který je tvořen z koeficientů účelové funcke,
 *          0, které reprezentují hodnoty slack proměnných a -M, které reprezentují hodnoty artifical proměnných.
 * \param num_bounds Počet omezení.
 * \param num_vars Počet proměnných.
 * \param objective Koeficienty účelové funkce
 * \param operators Operátory omezovacích funkcí.
 * \return Ukazatel (pole) na doubly, reprecentující C-row.
 */
double* simplex_prepare_c_row(const int num_bounds, const int num_vars, double objective[], char operators[][LINE_LENGTH]);

/**
 * \brief simplex_prepare_basis_column Funkce připraví bázový sloupec hodnot 0, které reprezentují hodnoty slack proměnných
 *           a -M, které reprezentují hodnoty artifical proměnných.
 *          Záleží na pouužitých operátorech.
 * \param num_bounds Počet omezení.
 * \param operators Operátory omezovacích funkcí.
 * \return Ukazatel (pole) na doubly, reprecentující Basis-column.
 */
double* simplex_prepare_basis_column(const int num_bounds, char operators[][LINE_LENGTH]);

/**
 * \brief simplex_prepare_z_row Funkce připraví Z řádek, který je tvořen součty součinů mezi prvky 'Basis-colum'
 *           a hodnot na řádkách v Simplexovo matici.
 * \param num_vars Počet proměnných.
 * \param num_bounds Počet omezení.
 * \param basis_column Bázový řádek.
 * \param simplex Simplexova matice.
 * \return Ukazatel (pole) na doubly, reprecentující Z-row.
 */
double* simplex_prepare_z_row(const int num_vars, const int num_bounds, double basis_column[], double* simplex[]);

/**
 * \brief simplex_prepare_c_z_row Funkce připraví 'C-Z' řádek, který je tvořen z rozdílů mezi prvky C-row a Z-row.
 * \param n Velikost 'C-row' a 'Z-row'
 * \param c_row Řádek 'C'
 * \param z_row Řádek 'Z'
 * \return Ukazatel (pole) na doubly, reprecentující 'C-Z-row'.
 */
double* simplex_prepare_c_z_row(const int n, double c_row[], double z_row[]);

/**
 * \brief simplex_check_optimal_solution Funkce zkontroluje jestli je alespoň jeden prvek v řádku 'C-Z' nenulový a kladný.
 * \param c_z_row Řádek 'C-Z'.
 * \param n Velikost 'C-Z-row'.
 * \return Vrací: 1 - True, 0 - False.
 */
int simplex_check_optimal_solution(double c_z_row[], const int n);

/**
 * \brief simplex_find_pivot Funkce najde adresu prvku mezi 'C-Z-row', který je největším kladným číslem.
 * \param c_z_row Řádek 'C-Z'.
 * \param n Velikost 'C-Z-row'.
 * \return Adresa největšího prvku.
 */
int simplex_find_pivot(double c_z_row[], const int n);

/**
 * \brief simplex_find_basis_replace Funkce najde nejmenší kladný podíl mezi pravou simplexovo tabulky a jejími prvky ze sloupce,
 *          určenným pivotním indexem. A najde jeho lokaci ve sloupci.
 * \param rh Pravá strana simplexovo tabulky.
 * \param pivot_index Pivotní index sloupce.
 * \param num_bounds Počet omezení.
 * \param simplex Simplexova tabulka.
 * \return Adresa prvku, který je největší dělitel své pravé strany.
 */
int simplex_find_basis_replace(double rh[], const int pivot_index, const int num_bounds, double* simplex[]);


#endif