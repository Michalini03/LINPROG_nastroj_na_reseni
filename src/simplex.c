#include "simplex.h"

double ** simplex_prepare_simplex_table(const int num_bounds, const int num_vars, double constraints[MAX_ROWS][MAX_VARS], char operators[][LINE_LENGTH]) {
    int i, j;
    double **simplex;
    
    if (num_bounds <= 0 || num_vars <= 0 || constraints == NULL || operators == NULL) {
        return NULL;
    }
    
    simplex = malloc(num_bounds * sizeof(double*));
    
    if (simplex == NULL) {
        return NULL;
    }
    
    for (i = 0; i < num_bounds; ++i) {
        simplex[i] = malloc((num_vars + num_bounds * 2) * sizeof(double));
        if (simplex[i] == NULL) {
            return NULL;
        }
    }

    /* Doplnění normálních hodnot a hodnot pro Slack a Artificial proměnné */
    for (i = 0; i < num_bounds; i++) {
        for (j = 0; j < num_vars; j++) {
            simplex[i][j] = constraints[i][j];
        }
        if (strcmp(operators[i],"<=") == 0) {
            simplex[i][i + num_vars] = 1;
        }
        else if (strcmp(operators[i], ">=") == 0) {
            simplex[i][i + num_vars] = -1;
            simplex[i][i + num_vars + num_bounds] = 1;
        }
        else if (strcmp(operators[i],"=") == 0) {
            simplex[i][i + num_vars + num_bounds] = 1;
        } 
    }

    return simplex;
}

double* simplex_prepare_c_row(const int num_bounds, const int num_vars, double objective[], char operators[][LINE_LENGTH]) {
    double* c_row;
    int i;

    if (num_bounds <= 0 || num_vars <= 0 || objective == NULL || operators == NULL) {
        return NULL;
    }

    c_row = (double*)malloc((num_vars + (num_bounds*2)) * sizeof(double));
    
    if (c_row == NULL) {
        return NULL;
    }

    for (i = 0; i < num_vars; i++) {
        c_row[i] = objective[i];
    }

    for (i = num_vars; i < num_vars + num_bounds; i++) {
        c_row[i] = 0;
    }

    for (i = 0; i < num_bounds; i++) {
        if (strcmp(operators[i], "<=")) {
            c_row[num_bounds + num_vars + i] = -M;
        }
        else {
            c_row[num_bounds + num_vars + i] = 0;
        }
    }
    return c_row;
}

double* simplex_prepare_basis_column(const int num_bounds, char operators[][LINE_LENGTH]) {
    double* basis_column;
    int i, n;
    
    if (num_bounds <= 0 || operators == NULL) {
        return NULL;
    }

    basis_column = (double*)malloc(num_bounds * sizeof(double));
    if (basis_column == NULL) {
        return NULL;
    }

    n = num_bounds;

    for (i = 0; i < n; i++) {
        if (strcmp(operators[i], "<=") == 0) {
            basis_column[i] = 0;
        }
        else {
            basis_column[i] = -M;
        }
    }

    return basis_column;
}

double* simplex_prepare_z_row(const int num_vars, const int num_bounds, double basis_column[], double* simplex[]) {
    double* z_row;
    int i, j, n;
    
    if (num_vars <= 0 || num_bounds <= 0 || basis_column == NULL || simplex == NULL) {
        return NULL;
    }

    z_row = (double*)malloc((num_vars + (num_bounds*2)) * sizeof(double));
    if (z_row == NULL) {
        return NULL;
    }

    n = num_vars + (num_bounds * 2);

    for (i = 0; i < n; i++) {
        z_row[i] = 0;
        for (j = 0; j < num_bounds; j++) {
            z_row[i] += basis_column[j] * simplex[j][i];
        }
    }

    return z_row;
}

double* simplex_prepare_c_z_row(const int n, double c_row[], double z_row[]) {
    double* c_z_row;
    int i;

    if (n <= 0 || c_row == NULL || z_row == NULL) {
        return NULL;
    }

    c_z_row = (double*)malloc(n * sizeof(double));
    if (c_z_row == NULL) {
        return NULL;
    }
    
    

    for (i = 0; i < n; i++) {
        c_z_row[i] = c_row[i] - z_row[i];
    }

    return c_z_row;
}

int simplex_check_optimal_solution(double c_z_row[], const int n) {
    int i;
    
    if (c_z_row == NULL || n <= 0) {
        return 0;
    }

    
 
    for (i = 0; i < n; i++) {
        if (c_z_row[i] > EPSILON) {
            return 1;
        }
    }
    return 0;
}

int simplex_find_pivot(double c_z_row[], const int n) {
    int i, max_index;
    double max_value;
    
    if (n <= 0 || c_z_row == NULL) {
        return -1;
    }

    max_value = -1;
    max_index = -1;

    for (i = 0; i < n; i++) {
        if (c_z_row[i] > max_value) {
            max_value = c_z_row[i];
            max_index = i;
        }
    }

    return max_index;
}

int simplex_find_basis_replace(double rh[], const int pivot_index, const int num_bounds, double* simplex[]) {
    int i, replace_basis_index, n;
    double min_value;
    
    if (simplex == NULL || rh == NULL || pivot_index < 0 || num_bounds <= 0) {
        return -1;
    }

    min_value = -1;
    replace_basis_index = -1;
    n = num_bounds;

    for (i = 0; i < n; i++) {
        if (simplex[i][pivot_index] > 0) {
            double ratio = rh[i] / simplex[i][pivot_index];
            if (min_value < 0 || ratio < min_value) {
                min_value = ratio;
                replace_basis_index = i;
            }
        }
    }

    return replace_basis_index;
}
