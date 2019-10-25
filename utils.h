#ifndef UTILS_H
#define UTILS_H

typedef struct{
    int nRegions;
    int nCities;
    int nStudents;
    int seed;
} Input;

// This typedef was created in order to simplify syntax
typedef int** Region;

// Read input from stdin and store it in data
void readInput(Input* data);
// Alocate and return a matrix, if error return NULL
int** matrix_new(int rows, int cols);
// Alocate and return a matrix, if error return NULL (double version)
double** matrix_new_double(int rows, int cols);
// Free a matrix allocated by matrix_new
void matrix_delete(int** m);
// Free a matrix allocated by matrix_new (double version)
void matrix_delete_double(double** m);
// Print a matrix
void matrix_print(int** m, int rows, int cols);
// Generate the regions
Region* generateRegions(Input* input, int mod);
// Return an array of regions of size specified in input
double*** allocateForMeasuresByCity(Input* input, int nMeasures);
// Free array allocated by generateRegions
void freeRegions(Region* regions, int nRegions);
// Free array allocated by allocateForMeasuresByCity
void freeMeasuresByCity(double*** regions, int nRegions);
// Fill measures by city
void fillMeasuresByCity(Region* regions, double*** measuresByCity, Input* input, int maxGrade);
// Print the measures by city
void printMeasuresByCity(double*** measuresByCity, Input* input);

#endif // UTILS_H
