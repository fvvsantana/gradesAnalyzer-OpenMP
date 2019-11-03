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

/*
 * Allocate an array of pointers to regions and a matrix to each region. Each
 * element of the array of regions points to a matrix.
 * Inside each matrix, we'll organize the data in a way that each column
 * represents a city and each row represents a measure.
 * In this way, if we attribute the returning structure of this function to a
 * variable called double*** measuresByCity, the element measuresByCity[i][j][k]
 * will be a double value that represents the "measure j" of the "city k" in the
 * "region i".
 * This manner of organization was chosen with the goal of putting all the
 * measures of the same type sequentially in the memory. For example, all
 * averages of cities in a single region will be sequential in the memory. This
 * is advantageous, because in the next step we'll calculate the average of the
 * averages of cities in a region to calculate a single average by region. The
 * fact that the elements are sequential in the memory can improve the use of
 * cache memory while the calculation of the average.
 * The same effect of optimization is expected to another measures like minimum
 * and maximum. But this effect won't be used in the calculation of median and
 * standard deviation, because we can't easily calculate the standard deviation
 * of a region using the standard deviation of the cities.
*/
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

/*
 * Return a matrix to store the measures of min, max, median, mean and standard
 * deviation by region. Each row is a measure, each column is a region.
 *
*/
double** allocateForMeasuresByRegion(Input* input, int nMeasures){
    return matrix_new_double(nMeasures, input->nRegions);
}

// Return an array to store the measures of a country
double* allocateForMeasuresByCountry(int nMeasures){
    return (double *) malloc(sizeof(double) * nMeasures);
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
 * is measures[regionIndex][measureIndex][cityIndex].
 * This convention was adopted because it will facilitate the next step.
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

/*
 * Fill the matrix with measures by region. The convention to access the measures
 * is [measureIndex][regionIndex].
*/
void fillMeasuresByRegion(Region* regions, double*** measuresByCity, double** measuresByRegion, Input* input, int maxGrade){
    int j;
    for(j=0; j<input->nRegions; j++){
        measuresByRegion[0][j] = find_min_double(measuresByCity[j][0], input->nCities);
        measuresByRegion[1][j] = find_max_double(measuresByCity[j][1], input->nCities);
		measuresByRegion[2][j] = find_median(regions[j][0], (input->nCities) * (input->nStudents), maxGrade+1);
        measuresByRegion[3][j] = calculate_average_double(measuresByCity[j][3], input->nCities);
		measuresByRegion[4][j] = calculate_stddev(regions[j][0], (input->nCities) * (input->nStudents));
    }
}

/*
 * Fill the array with measures of a country. The convention to access the measures
 * is [measureIndex].
*/
void fillMeasuresByCountry(double** measuresByRegion, double* measuresByCountry, Input* input, int maxGrade){
    measuresByCountry[0] = find_min_double(measuresByRegion[0], input->nRegions);
    measuresByCountry[1] = find_max_double(measuresByRegion[1], input->nRegions);
    // This calculation is wrong:
	//measuresByCountry[2] = find_median_double(measuresByRegion[2], input->nRegions, maxGrade+1);
    measuresByCountry[3] = calculate_average_double(measuresByRegion[3], input->nRegions);
    // This calculation is wrong:
	//measuresByCountry[4] = calculate_stddev_double(measuresByRegion[4], input->nRegions);
}

// Get the region that has the best average
int getBestRegion(double** measuresByRegion){
    return find_pos_of_max_double(measuresByRegion[3], measuresByRegion[4] - measuresByRegion[3]);
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

// Print the measures by region
void printMeasuresByRegion(double** measuresByRegion, Input* input){
    int j;
    for(j=0; j<input->nRegions; j++){
        printf("Reg %d: menor: %d, maior: %d, mediana: %.2lf, media: %.2lf e DP: %.2lf\n",
            j,
            (int) measuresByRegion[0][j],
			(int) measuresByRegion[1][j],
			measuresByRegion[2][j],
			measuresByRegion[3][j],
			measuresByRegion[4][j]
        );
    }
}

// Print the measures by country
void printMeasuresByCountry(double* measuresByCountry){
    printf("\nBrasil: menor: %d, maior: %d, mediana: %.2lf, media: %.2lf e DP: %.2lf\n",
		(int) measuresByCountry[0],
		(int) measuresByCountry[1],
		measuresByCountry[2],
		measuresByCountry[3],
		measuresByCountry[4]
    );
}
