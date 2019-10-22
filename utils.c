#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Alocate a matrix
int** matrix_new(int rows, int cols){
    int** m;

    // Alocation
    m = (int **) malloc(sizeof(int *) * rows);
    m[0] = (int *) malloc(sizeof(int) * rows * cols);
    if(!(m && m[0])){
        fprintf(stderr, "ERROR: Bad allocation!\n");
    }

    // Linkage:
    int i;
    for (i = 1; i < rows; i++) {
        m[i] = m[0] + i * cols;
    }

    return m;
}

// Free a matrix allocated by matrix_new
void matrix_delete(int** m){
    if(m){
        free(m[0]);
    }
    free(m);
}
