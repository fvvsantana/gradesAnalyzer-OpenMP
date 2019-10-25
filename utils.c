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

/*
 * Return an array of regions, where each region is a matrix, each row of this
 * matrix is a city, each cell of the city is a student.
 * The grades of the students is generated using the seed inside the input structure.
 * The grades are generated between 0 and mod-1.
*/
Region* generateRegions(Input* input, int mod){
    // Set seed
    srand((unsigned) input->seed);
    // Array of regions, where a region is basically a matrix
    Region* regions = (Region *) malloc(sizeof(Region) * input->nRegions);
    int i, j, k;
    // For each region
    for(i=0; i<input->nRegions; i++){
        // Allocate a region
        regions[i] = matrix_new(input->nCities, input->nStudents);

        // Fill matrix
        for(j=0; j<input->nCities; j++){
            for(k=0; k<input->nStudents; k++){
                regions[i][j][k] = rand()%mod;
            }
        }
    }
    // Return regions array
    return regions;
}
// Return an array of regions of size specified in input
Region* allocateForMeasuresByCity(Input* input){
    // Array of regions, where a region is basically a matrix
    Region* regions = (Region *) malloc(sizeof(Region) * input->nRegions);
    int i;
    // For each region
    for(i=0; i<input->nRegions; i++){
        // Allocate a region
        regions[i] = matrix_new(input->nCities, input->nStudents);
    }
    // Return regions array
    return regions;
}

// Free array allocated by generateRegions or by allocateForMeasuresByCity
void freeRegions(Region* regions, int nRegions){
    int i;
	// Free data matrices
	for(i=0; i<nRegions; i++){
		matrix_delete(regions[i]);
	}
	// Free regions array
	free(regions);
}
