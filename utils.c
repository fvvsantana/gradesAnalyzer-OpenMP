#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// Alocate and return a matrix, if error return NULL
int** matrix_new(int rows, int cols){
    int** m;

    // Alocation
    // Array of pointers
    m = (int **) malloc(sizeof(int *) * rows);
    // Integers
    m[0] = (int *) malloc(sizeof(int) * rows * cols);
    if(!(m && m[0])){
        fprintf(stderr, "ERROR: Bad allocation!\n");
        return NULL;
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
        // Free integers
        free(m[0]);
    }
    // Free array of pointers
    free(m);
}

// Print a matrix
void matrix_print(int** m, int rows, int cols){
    int i, j;
    for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
            printf("%d\t", m[i][j]);
        }
        printf("\n");
    }
}

// Read input from stdin and store it in data
void readInput(Input* data){
    // Read data
    scanf("%d %d %d %d", &data->nRegions, &data->nCities, &data->nStudents, &data->seed);
}
