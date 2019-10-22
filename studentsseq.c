#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utils.h"

#define FILENAME "input.in"

// This typedef was created in order to simplify syntax
typedef int** Region;

int main(){
    // Read input
    Input input;
    if(!readInput(FILENAME, &input)){
        exit(1);
    }

    // Create all matrices
    srand((unsigned) input.seed);
    // Array of regions, where a region is basically a matrix
    Region* regions = (Region *) malloc(sizeof(Region) * input.nRegions);
    int i, j, k;
    // For each region
    for(i=0; i<input.nRegions; i++){
        // Allocate a region
        regions[i] = matrix_new(input.nCities, input.nStudents);

        // Fill matrix
        for(j=0; j<input.nCities; j++){
            for(k=0; k<input.nStudents; k++){
                regions[i][j][k] = rand()%101;
            }
        }
    }


    // TODO: Calculation, storage and printing





    return 0;
}
