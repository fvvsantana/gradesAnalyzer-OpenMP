#include <stdio.h>
#include <omp.h>
#include "utils.h"

int main(){

    int** matrix;
    matrix = matrix_new(3, 3);
    matrix[0][0] = 5;
    printf("oi\n");
    printf("%d\n", matrix[0][0]);
    printf("%d\n", matrix[2][0]);
    printf("%d\n", matrix[2][0]);
    printf("%d\n", matrix[0][0]);
    matrix_delete(matrix);

    return 0;
}
