#ifndef SIMPLEX_H
#define SIMPLEX_H
#define LINE_LENGTH 256
#define M 1000
#define EPSILON 0.000000001

double* simplex_prepare_c_row(const int num_bounds, const int num_vars, double objective[], char operators[][LINE_LENGTH]);

double* simplex_preparace_basis_column(const int num_bounds, char operators[][LINE_LENGTH]);

double* simplex_prepare_z_row(const int num_vars, const int num_bounds, double basis_column[], double* simplex[]);

double* simplex_prepare_c_z_row(const int n, double c_row[], double z_row[]);

int simplex_check_optimal_solution(double c_z_row[], const int n);

int simplex_find_pivot(double c_z_row[], const int n);

int simplex_find_basis_replace(double rh[], const int pivot_index, const int num_bounds, double* simplex[]);


#endif