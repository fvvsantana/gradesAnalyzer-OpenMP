#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "statisticsseq.h"


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

// Alocate and return a matrix, if error return NULL (double version)
double** matrix_new_double(int rows, int cols){
    double** m;

    // Alocation
    // Array of pointers
    m = (double **) malloc(sizeof(double *) * rows);
    // Integers
    m[0] = (double *) malloc(sizeof(double) * rows * cols);
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

// Free a matrix allocated by matrix_new (double version)
void matrix_delete_double(double** m){
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
double*** allocateForMeasuresByCity(Input* input, int nMeasures){
    // Array of regions, where a region is basically a matrix
    double*** regions = (double ***) malloc(sizeof(double**) * input->nRegions);
    int i;
    // For each region
    for(i=0; i<input->nRegions; i++){
        // Allocate a region
        regions[i] = matrix_new_double(nMeasures, input->nCities);
    }
    // Return regions array
    return regions;
}

// Free array allocated by generateRegions
void freeRegions(Region* regions, int nRegions){
    int i;
	// Free data matrices
	for(i=0; i<nRegions; i++){
		matrix_delete(regions[i]);
	}
	// Free regions array
	free(regions);
}

// Free array allocated by allocateForMeasuresByCity
void freeMeasuresByCity(double*** regions, int nRegions){
    int i;
	// Free data matrices
	for(i=0; i<nRegions; i++){
		matrix_delete_double(regions[i]);
	}
	// Free regions array
	free(regions);
}

/*
 * Fill the regions with taken the measures. The convention to access the measures
 * are measures[regionIndex][measureIndex][cityIndex].
 * This convention was adopted becase it will facilitate the next step.
 * The next step is take the measures by region, so we'll need to sum up all the
 * cities from a region. So it's better having the cities sequencially in the
 * memory.
*/
void fillMeasuresByCity(Region* regions, double*** measuresByCity, Input* input, int maxGrade){
    int i, j;
    for(i=0; i<input->nRegions; i++){
        for(j=0; j<input->nCities; j++){
            measuresByCity[i][0][j] = find_min(regions[i][j], input->nStudents);
			measuresByCity[i][1][j] = find_max(regions[i][j], input->nStudents);
			measuresByCity[i][2][j] = find_median(regions[i][j], input->nStudents, maxGrade+1);
			measuresByCity[i][3][j] = calculate_average(regions[i][j], input->nStudents);
			measuresByCity[i][4][j] = calculate_stddev(regions[i][j], input->nStudents);
        }
    }
}

// Print the measures by city
void printMeasuresByCity(double*** measuresByCity, Input* input){
    int i, j;
    for(i=0; i<input->nRegions; i++){
        for(j=0; j<input->nCities; j++){
            printf("Reg %d - Cid %d: menor: %d, maior: %d, mediana: %.2lf, media: %.2lf e DP: %.2lf\n",
                i, j,
                (int) measuresByCity[i][0][j],
    			(int) measuresByCity[i][1][j],
    			measuresByCity[i][2][j],
    			measuresByCity[i][3][j],
    			measuresByCity[i][4][j]
            );
        }
        printf("\n");
    }

}
