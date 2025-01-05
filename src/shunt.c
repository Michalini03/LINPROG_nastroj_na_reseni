#include "shunt.h"

/* Zásobník pro operátory */
char stack[MAX_STACK_SIZE];
int stack_top = -1;

/* Fronta pro výstupní postfix výraz */
char queue[MAX_QUEUE_SIZE][50];
int queue_rear = -1;

void shunt_push(char c) {
    if (stack_top >= MAX_STACK_SIZE - 1) {
        printf("Memory overflow!\n");
        return;
    }

    stack[++stack_top] = c;
}

char shunt_pop() {
    if (stack_top < 0) {
        printf("Stack empty!\n");
        return '\0';
    }

    return stack[stack_top--];
}

char shunt_peek() {
    if (stack_top < 0) {
        printf("Stack empty!\n");
        return '\0';
    }

    return stack[stack_top];
}

void shunt_enqueue(const char *token) {
    if (!token) {
        return;
    }

    if (queue_rear >= MAX_QUEUE_SIZE - 1) {
        printf("Queue overflow!\n");
        return;
    }
    strcpy(queue[++queue_rear], token);
}

int shunt_is_operator(char token) {
    return token == '*' || token == '/' || token == '+' || token == '-';
}

int shunt_is_open_bracket(char token) {
    return token == '[' || token == '{' || token == '(';
}

int shunt_is_closed_bracket(char token) {
    return token == ']' || token == '}' || token == ')';
}

int shunt_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

void shunt_print_postfix(char **postfix) {
    int i;
    if (!postfix || !*postfix) {
        return;
    }

    for (i = 0; i < queue_rear; i++) {
        printf("%s ", postfix[i]);
    }
    printf("\n");
}

int shunt_get_postfix(char *infix, char **postfix) {
    char last_char[50] = "";  /* Předposlední znak */
    char var_name[50] = "";  /* Dočasný název proměnné nebo hodnota čísla */
    int var_index = 0;
    char op[2];
    char add;

    int i;

    stack_top = -1;
    queue_rear = -1;

    for (i = 0; infix[i] != '\0'; i++) {
        char ch = infix[i];

        /* Ověření záporných čísel */
        if (ch == '-' && (strcmp(last_char, "") == 0 || shunt_is_operator(last_char[0]) || shunt_is_closed_bracket(last_char[0]) || shunt_is_open_bracket(last_char[0]))){
            shunt_enqueue("-1");
            shunt_push('*');
            continue;
        }

        /* Implicitní násobení (např. `2x`) */
        if (strcmp(last_char, var_name) == 0 && infix[i - 1] == ' ' && isdigit(ch) && strcmp(last_char, "") != 0) {
            shunt_push('*');
            shunt_enqueue(var_name);
            strncpy(last_char, var_name, var_index);
            var_index = 0;
        }

        /* Zpracování čísel */
        if (isdigit(ch) || ch == '.') {
            var_name[var_index++] = ch;
            strncpy(last_char, var_name, var_index);
            continue;
        }

        /* Zpracování proměnných */
        if (isalpha(ch) || ch == '_' ) {
            if (isdigit(var_name[0])) {
                shunt_enqueue(var_name);
                shunt_push('*');
                memset(var_name, '\0', sizeof(var_name)); 
                var_index = 0;
            }
            var_name[var_index++] = ch;
            strncpy(last_char, var_name, var_index);
            continue;
        }

        /* Implicitní násobení (např. `)(`) */
        if (shunt_is_closed_bracket(last_char[0]) && shunt_is_open_bracket(ch)) {
            shunt_push('*');
        }

        /* Operátor */
        if (shunt_is_operator(ch)) {
            if(shunt_is_operator(last_char[0])) {
                return 11;
            }
            if (var_index > 0) {
                var_name[var_index] = '\0';
                shunt_enqueue(var_name);
                memset(var_name, '\0', sizeof(var_name)); 
                var_index = 0;
            }
            while (stack_top >= 0 && shunt_precedence(shunt_peek()) >= shunt_precedence(ch)) {
                op[0] = shunt_pop();
                op[1] = '\0';
                shunt_enqueue(op);
            }
            shunt_push(ch);
            last_char[0] = ch;
            last_char[1] = '\0';
            continue;
        }

        /* Otevírací závorka */
        if (shunt_is_open_bracket(ch)) {
            if (var_index > 0) {
                var_name[var_index] = '\0';
                shunt_enqueue(var_name);
                memset(var_name, '\0', sizeof(var_name)); 
                var_index = 0;
                add = '*';
                shunt_push(add);
            }
            shunt_push(ch);
            last_char[0] = ch;
            last_char[1] = '\0';
            continue;
        }

        /* Zavírací závorka */
        if (shunt_is_closed_bracket(ch)) {
            if (shunt_is_operator(last_char[0])) {
                return 11;
            }
            if (var_index > 0) {
                var_name[var_index] = '\0';
                shunt_enqueue(var_name);
                memset(var_name, '\0', sizeof(var_name)); 
                var_index = 0;
            }
            while (stack_top >= 0 && !shunt_is_open_bracket(shunt_peek())) {
                op[0] = shunt_pop();
                op[1] = '\0';
                shunt_enqueue(op);
            }
            if (stack_top < 0 || shunt_peek() != (ch == ')' ? '(' : ch == ']' ? '[' : '{')) {
                return 1;
            }
            shunt_pop();
            last_char[0] = ch;
            last_char[1] = '\0';
            continue;
        }

        /* Ignorovat mezery */
        if (ch == ' ') continue;
        
        /* Pokud se dostanu až sem, jedná se o nepovolený charakter */
        return 1;
    }

    /* Zpracování zbylých proměnných */
    if (var_index > 0) {
        var_name[var_index] = '\0';
        shunt_enqueue(var_name);
    }

    /* Přesun zbylých operátorů ze zásobníku */
    while (stack_top >= 0) {
        if (shunt_is_open_bracket(shunt_peek())) {
            printf("Error: Unmatched brackets\n");
            return 1;
        }
        op[0] = shunt_pop();
        op[1] = '\0';
        shunt_enqueue(op);
    }
    queue_rear += 1;

    for(i = 0; i < queue_rear; i++) {
        strcpy(postfix[i], queue[i]);
    }
    return 0;
}

char **shunt_alloc_postfix() {
    int i, j;

    char **postfix = malloc(MAX_QUEUE_SIZE * sizeof(char *));
    if (!postfix) {
        return NULL;
    }

    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
        postfix[i] = malloc(50 * sizeof(char));
        if (!postfix[i]) {

            for (j = 0; j < i; j++) {
                free(postfix[j]);
            }
            free(postfix);

            return NULL;
        }
    }

    return postfix;
}

void shunt_free_postfix(char **postfix) {
    int i;

    if (!postfix) return;

    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
        free(postfix[i]);
    }
    free(postfix);
}


Expression postfix_create_expression() {
    Expression expr;

    expr.size = 0;
    expr.capacity = 10;
    expr.terms = malloc(expr.capacity * sizeof(Term));

    if (!expr.terms) {
        expr.size = -1;
        expr.capacity = -1;
        expr.terms = NULL;
    }

    return expr;
}


void postfix_free_expression(Expression *expr) {
    int i;
    if (!expr) {
        return;
    }

    for ( i = 0; i < expr->size; i++) {
        free(expr->terms[i].variable);
    }
    free(expr->terms);
    expr->size = 0;
    expr->capacity = 0;
}


void postfix_add_term(Expression *expr, const char *variable, double coefficient) {
    if (expr->size == expr->capacity) {
        expr->capacity *= 2;
        expr->terms = realloc(expr->terms, expr->capacity * sizeof(Term));
    }

    if(variable) {
        expr->terms[expr->size].variable = malloc(50);
        strcpy(expr->terms[expr->size].variable, variable);
        
    }
    else {
        expr->terms[expr->size].variable = NULL;
    }
    expr->terms[expr->size].coefficient = coefficient;
    expr->size++;
}


Expression postfix_apply_operator(char op, Expression left, Expression right) {
    int i;
    Expression result = postfix_create_expression();

    if (result.size < 0) {
        return result;
    }

    if (op == '+') {
        if (!left.terms[0].variable && !right.terms[0].variable) {
            /* Sjednocení jednoduchých výrazů (např 2 + 7 = 9)*/
            result.terms[0].variable = NULL;
            result.terms[0].coefficient = left.terms[0].coefficient + right.terms[0].coefficient;
            result.size = 1;
        }
        else {
            for (i = 0; i < left.size; i++) {
                postfix_add_term(&result, left.terms[i].variable, left.terms[i].coefficient);
            }
            for (i = 0; i < right.size; i++) {
                postfix_add_term(&result, right.terms[i].variable, right.terms[i].coefficient);
            }
        }
    } else if (op == '-') {
        if (!left.terms[0].variable && !right.terms[0].variable) {
            result.terms[0].variable = NULL;
            result.terms[0].coefficient = left.terms[0].coefficient - right.terms[0].coefficient;
            result.size = 1;
        }
        else {
            for (i = 0; i < left.size; i++) {
                postfix_add_term(&result, left.terms[i].variable, left.terms[i].coefficient);
            }
            for (i = 0; i < right.size; i++) {
                postfix_add_term(&result, right.terms[i].variable, -right.terms[i].coefficient);
            }
        }
    } else if (op == '*') {
        if (left.size == 1 && left.terms[0].variable == NULL) {
            double constant = left.terms[0].coefficient;

            if (right.size == 1 && right.terms[0].variable == NULL) {
                result.terms[0].variable = NULL;
                result.terms[0].coefficient = left.terms[0].coefficient * right.terms[0].coefficient;
                result.size = 1;
            }
            else {
                for (i = 0; i < right.size; i++) {
                postfix_add_term(&result, right.terms[i].variable, right.terms[i].coefficient * constant);
                }
            }
        } else if (right.size == 1 && right.terms[0].variable == NULL) {
            double constant = right.terms[0].coefficient;

            for (i = 0; i <= left.size; i++) {
                postfix_add_term(&result, left.terms[i].variable, left.terms[i].coefficient * constant);
            }
        } else {
            result.capacity = -1;
            result.size = -1;
            result.terms = NULL;
            return result;
        }
    } else if (op == '/') { 
      if (left.size == 1 && left.terms[0].variable == NULL) {
            double constant = left.terms[0].coefficient;
            
            if (right.size == 1 && right.terms[0].variable == NULL) {
                result.terms[0].variable = NULL;
                result.terms[0].coefficient = left.terms[0].coefficient / right.terms[0].coefficient;
                result.size = 1;
            }
            else {
                for (i = 0; i < right.size; i++) {
                postfix_add_term(&result, right.terms[i].variable, constant / right.terms[i].coefficient);
                }
            }
        } else if (right.size == 1 && right.terms[0].variable == NULL) {
            double constant = right.terms[0].coefficient;
            
            for (i = 0; i < left.size; i++) {
                postfix_add_term(&result, left.terms[i].variable, constant / left.terms[i].coefficient);
            }
        } else {
            result.capacity = -1;
            result.size = -1;
            result.terms = NULL;
            return result;
        }
    }
    else {
        result.capacity = -1;
        result.size = -1;
        result.terms = NULL;
        return result;
    }

    return result;
}

Expression postfix_parse_expression(char **tokens) {
    Expression expr_stack[100];             /**< Zásobník pro výrazy, když narazíme na logický operátor */
    Expression expr, right, left, result;   /**< Potřebné výrazy pro naši práci */
    int i, j, stack_size, token_count;

    /* Jelikož funkce vyhazuje strukturu Expression, tak vyhodím chybný a zbytečný výraz */
    if(!tokens || !*tokens) {
        expr_stack[0].size = -1;
        expr_stack[0].capacity = -1;
        expr_stack[0].terms = NULL;
        return expr_stack[0];
    }
    
    stack_size = 0;
    token_count = queue_rear; 

    for (i = 0; i < token_count; i++) {
        const char *token = tokens[i];

        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {      /* Pokud se jedná o proměnnou či číslo, vytvořím nový expression */
            expr = postfix_create_expression();
            if (expr.size < 0) {
                return expr;
            }

            postfix_add_term(&expr, NULL, atof(token));
            expr_stack[stack_size++] = expr;
        } else if (shunt_is_operator(token[0]) && token[1] == '\0') {
            /** Pokud už nemám dva výrazy nad kterými mohu zavolat operátor, 
             * tak se někde stala chyba
             * */
            if (stack_size < 2) {
                for (i = 0; i < stack_size; i++) {
                    for (j = 0; j < expr_stack[i].size; j++) {
                        free(expr_stack[i].terms[j].variable); 
                    }
                    free(expr_stack[i].terms);
                }
                expr_stack[0].size = -1;
                expr_stack[0].capacity = -1;
                expr_stack[0].terms = NULL;
                return expr_stack[0];
            }

            /* Popnu ze stacku dva nové výrazy, záleží na posloupnosti! */
            right = expr_stack[--stack_size];
            left = expr_stack[--stack_size];

            result = postfix_apply_operator(token[0], left, right);
            postfix_free_expression(&left);
            postfix_free_expression(&right);

            if (result.size < 0) {
                return result;
            }

            expr_stack[stack_size++] = result;
        } else {
            expr = postfix_create_expression();
            postfix_add_term(&expr, token, 1.0);
            expr_stack[stack_size++] = expr;
        }
    }

    /* Vyprázdním zásobník */
    if (stack_size != 1) {
        for(i = 0; i < stack_size; i++) {
            for(j = 0; j < expr_stack[i].size; j++) {
                free(expr_stack[i].terms[j].variable); 
            }
            free(expr_stack[i].terms);
        }
        expr_stack[0].size = -1;
        expr_stack[0].capacity = -1;
        expr_stack[0].terms = NULL;
        return expr_stack[0];
    }

    return expr_stack[--stack_size];
}

void postfix_print_expression(const Expression *expr) {
    int i;
    if (!expr) {
        return;
    }

    for (i = 0; i < expr->size; i++) {
        if (expr->terms[i].variable) {
            printf("%s: %.2f\n", expr->terms[i].variable, expr->terms[i].coefficient);
        } else {
            printf("Constant: %.2f\n", expr->terms[i].coefficient);
        }
    }
}