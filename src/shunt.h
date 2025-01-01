#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef SHUNT_H
#define SHUNT_H

/** Maximální velikost zásobníku */
#define MAX_STACK_SIZE 100
/** Maximální velikost fronty */
#define MAX_QUEUE_SIZE 200


/* VYTVOŘENÍ POSTFIXU */

/**
 * \brief shunt_push Funkce přidá charakter na zásobník.
 * \param c Předaný charakter.
 * \return Vrací void pointer.
 */
void shunt_push(char c);

/**
 * \brief shunt_pop Funkce vrací poslední přidaný prvek na zásobníku a vyndavá jej z něj.
 * \return Poslední znak v zásobníku.
 */
char shunt_pop();

/**
 * \brief shunt_peek Funkce vrací poslední přidaný prvek na zásobníku.
 * \return Poslední znak v zásobníku.
 */
char shunt_peek();

/**
 * \brief shunt_enqueue Funkce přidává prvek do fronty.
* \param token Řetězec, reprezentující daný matematický prvek. (Operátor, číslo, proměnnou)
 * \return Vrací void pointer.
 */ 
void shunt_enqueue(const char *token);

/**
 * \brief shunt_is_operator Funkce zkontroluje, jestli předaný token je matematický operátor.
 * \param token Znak, předaný pro kontrolu.
 * \return Vrací 1 - True, 0 - False.
 */
int shunt_is_operator(char token);

/**
 * \brief shunt_is_open_bracket Funkce zkontroluje, jestli předaný token je začátek nějakého typu závorky.
 * \param token Znak, předaný pro kontrolu.
 * \return Vrací 1 - True, 0 - False.
 */
int shunt_is_open_bracket(char token);

/**
 * \brief shunt_is_closed_bracket Funkce zkontroluje, jestli předaný token je konec nějakého typu závorky.
 * \param token Znak, předaný pro kontrolu.
 * \return Vrací 1 - True, 0 - False.
 */
int shunt_is_closed_bracket(char token);

/**
 * \brief shunt_precendence Funkce vrací váhu předaného operátoru.
 * \param op Znak, reprezentující operátor.
 * \return Vrací 1 - +, - || 2 - *, / || 0 - chybný operátor
 */
int shunt_precedence(char op);

/**
 * \brief shunt_get_postfix Vytvoří z infixového zápisu zápis postfixový.
 * \param infix Infixový zápis.
 * \param postfix Ukazetel na řetězec, do kterého se bude ukládat postfixový zápis.
 * \return Číselná hodnota východu z funkce, pokud se vrátí nula, vše je v pořádku.
 */
int shunt_get_postfix(char* infix, char** postfix);

/**
 * \brief shunt_print_postfix Funkce vypíše postfixový zápis do konzole.
 * \param postfix Pointer na řetězec postfixu.
 * \return Vrací void pointer.
 */
void shunt_print_postfix(char **postfix);

/**
 * \brief shunt_alloc_postfix Funkce alokuje paměť potřebnou pro vytvoření postfixu.
 * \return Vrací ukazatel na řetězec.
 */
char **shunt_alloc_postfix();

/**
 * \brief shunt_free_postfix Funkce dealokuje paměť potřebnou pro vytvoření postfixu.
 * \param postfix Pointer na řetězec, který se bude uvolňovat.
 * \return Vrací void pointer.
 */
void shunt_free_postfix(char **postfix);


/* ZPRACOVÁNÍ POSTFIXU */
/**
 * \struct Term
 * \brief Struktura pro matematický prvek
 */
typedef struct {
    char *variable;         /**< Název proměnné. (V případě, že je prvek jen samostatné číslo -> NULL) */
    double coefficient;     /**< Číselná hodnota koeficientu. */
} Term;

/**
 * \struct Expression
 * \brief Pole kompletního výrazu, seskládaná z různých prvků.
 */
typedef struct {
    Term *terms;        /**< Pole prvků. */
    int size;           /**< Počet prvků v poli. */
    int capacity;       /**< Kapacita pole. */
} Expression;

/**
 * \brief postfix_create_expression Funkce vytváří prázdný výraz.
 * \return Lokálně vytvořený výraz. 
 */
Expression postfix_create_expression();

/**
 * \brief postfix_free_expression Funkce dealokuje předaný výraz.
 * \param expr Ukazatel na výraz.
 * \return Vrací void pointer.
 */
void postfix_free_expression(Expression *expr);

/**
 * \brief postfix_add_term Funkce alokuje paměť pro nový prvek, ten inicializuje a přidá do předaného výrazu.
 * \param expr Ukazatel na výraz, kam přidáme nový prvek.
 * \param variable Název proměnné nového prvku.
 * \param coefficient Hodnota koeficientu nového prvku.
 * \return Vrací void pointer.
 */
void postfix_add_term(Expression *expr, const char *variable, double coefficient);

/**
 * \brief postfix_apply_operator Funkce provede matematickou operaci podle předaného operátoru,
 *          a podle hodnot, které v sobě mají zbylé dva výrazy.
 * \param op Znak operátoru.
 * \param left Výraz z levé strany operátoru.
 * \param right Výraz z pravé strany operátoru.
 * \return Vrací nově vytvořený výraz.
 */
Expression postfix_apply_operator(char op, Expression left, Expression right);

/**
 * \brief postfix_parse_expression Funkce z předaného postfixového zápisu vytvoří jeden velký výraz.
 * \param tokens Pole řetězců reprezentující postfixový zápis.
 * \return Nově vytvořený výraz, který v sobě ukládá všechny hodnoty zápisu.
 */
Expression postfix_parse_expression(char **tokens);

/** 
 * \brief postfix_print_expression Funkce vypíše předaný výraz do konzole.
 * \param expr Ukazatel na výraz.
 * \return Funkce vrací void pointer.
 */
void postfix_print_expression(const Expression *expr);


#endif