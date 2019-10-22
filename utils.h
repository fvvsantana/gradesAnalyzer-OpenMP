#ifndef UTILS_H
#define UTILS_H

typedef struct{
    int nRegions;
    int nCities;
    int nStudents;
    int seed;
} Input;

int readInput(char* fileName, Input* data);

int** matrix_new(int rows, int cols);
void matrix_delete(int** m);

#endif // UTILS_H
