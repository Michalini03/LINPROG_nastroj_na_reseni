#include "simplex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        if(strcmp(operators[i], "<=")) {
            c_row[num_bounds + num_vars + i] = -M;
        }
        else {
            c_row[num_bounds + num_vars + i] = 0;
        }
    }
    return c_row;
}

double* simplex_preparace_basis_column(const int num_bounds, char operators[][LINE_LENGTH]) {
    double* basis_column;
    int i;
    
    if (num_bounds <= 0 || operators == NULL) {
        return NULL;
    }

    basis_column = (double*)malloc(num_bounds * sizeof(double));
    if (basis_column == NULL) {
        return NULL;
    }


    for (i = 0; i < num_bounds; i++) {
        if(strcmp(operators[i], "<=") == 0) {
            basis_column[i] = 0;
        }
        else {
            basis_column[i] = -M;
        }
    }

    return basis_column;
}

double* simplex_prepare_z_row(const int num_vars, const int num_bounds, double basis_column[], double simplex[num_bounds][num_vars + (num_bounds * 2)]) {
    double* z_row;
    int i, j;
    
    if (num_vars <= 0 || num_bounds <= 0 || basis_column == NULL || simplex == NULL) {
        return NULL;
    }

    z_row = (double*)malloc((num_vars + (num_bounds * 2)) * sizeof(double));
    if (z_row == NULL) {
        return NULL;
    }

    for (i = 0; i < num_vars + (num_bounds * 2); i++) {
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
        if (c_z_row[i] > 0) {
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

int simplex_find_basis_replace(double rh[], const int pivot_index, const int num_vars, const int num_bounds, double simplex[][num_vars + (num_bounds * 2)]) {
    int i, replace_basis_index;
    double min_value;
    
    if (simplex == NULL || rh == NULL || pivot_index < 0 || num_bounds <= 0) {
        return -1;
    }

    min_value = -1;
    replace_basis_index = -1;

    for (i = 0; i < num_bounds; i++) {
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
