#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// Alocate a matrix, if error return NULL
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

// Read input. Return 0 if error, else return 1
int readInput(char* fileName, Input* data){
    FILE* fp;
    // Open file
    fp = fopen(fileName, "r");
    if(!fp){
        fprintf(stderr, "ERROR: Cannot open file called: \"%s\"\n", fileName);
        return 0;
    }

    // Read data
    fscanf(fp, "%d %d %d %d", &data->nRegions, &data->nCities, &data->nStudents, &data->seed);
    return 1;
}
