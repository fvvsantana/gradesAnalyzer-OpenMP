#ifndef UTILSPAR_H
#define UTILSPAR_H

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
// Return a matrix to store measures by region
double** allocateForMeasuresByRegion(Input* input, int nMeasures);
// Return an array to store the measures of a country
double* allocateForMeasuresByCountry(int nMeasures);

// Free array allocated by generateRegions
void freeRegions(Region* regions, int nRegions);
// Free array allocated by allocateForMeasuresByCity
void freeMeasuresByCity(double*** regions, int nRegions);

// Fill measures by city
void fillMeasuresByCity(Region* regions, double*** measuresByCity, Input* input, int maxGrade);
// Fill measures by region
void fillMeasuresByRegion(Region* regions, double*** measuresByCity, double** measuresByRegion, Input* input, int maxGrade);
// Fill measures by country
void fillMeasuresByCountry(Region* regions, double** measuresByRegion, double* measuresByCountry, Input* input, int maxGrade);
// Get the region that has the best average
int getBestRegion(double** measuresByRegion);

// Print the measures by city
void printMeasuresByCity(double*** measuresByCity, Input* input);
// Print the measures by region
void printMeasuresByRegion(double** measuresByRegion, Input* input);
// Print the measures by country
void printMeasuresByCountry(double* measuresByCountry);

#endif // UTILSPAR_H
