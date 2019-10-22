#ifndef UTILS_H
#define UTILS_H

typedef struct{
    int nRegions;
    int nCities;
    int nStudents;
    int seed;
} Input;

// Read file content and store it in data
int readInput(char* fileName, Input* data);
// Alocate and return a matrix, if error return NULL
int** matrix_new(int rows, int cols);
// Free a matrix allocated by matrix_new
void matrix_delete(int** m);
// Print a matrix
void matrix_print(int** m, int rows, int cols);

#endif // UTILS_H
