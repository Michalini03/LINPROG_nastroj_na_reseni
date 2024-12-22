#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef SHUNT_H
#define SHUNT_H

#define MAX_STACK_SIZE 100
#define MAX_QUEUE_SIZE 200


/* VYTVOŘENÍ POSTFIXU */
void shunt_push(char c);

char shunt_pop();

char shunt_peek();

void shunt_enqueue(const char *token);

int shunt_is_operator(char token);

int shunt_is_open_bracket(char token);

int shunt_is_closed_bracket(char token);

int shunt_precedence(char op);

int shunt_get_postfix(char* infix, char** postfix);

void shunt_print_postfix(char **postfix);

char **shunt_alloc_postfix();

void shunt_free_postfix(char **postfix);


/* ZPRACOVÁNÍ POSTFIXU */
typedef struct {
    char *variable;
    double coefficient;
} Term;

typedef struct {
    Term *terms;
    int size;
    int capacity;
} Expression;

Expression postfix_create_expression();
void postfix_free_expression(Expression *expr);
void postfix_add_term(Expression *expr, const char *variable, double coefficient);
Expression postfix_apply_operator(char op, Expression left, Expression right);
Expression postfix_parse_expression(char **tokens);
void postfix_print_expression(const Expression *expr);


#endif