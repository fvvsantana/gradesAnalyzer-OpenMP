#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utils.h"

#define FILENAME "input.in"

int main(){
    Input input;
    if(!readInput(FILENAME, &input)){
        exit(1);
    }

    //printf("%d %d %d %d", input.nRegions, input.nCities, input.nStudents, input.seed);
    int** matrix;
    matrix = matrix_new(3, 3);
    matrix[0][0] = 5;
    matrix_delete(matrix);



    return 0;
}
