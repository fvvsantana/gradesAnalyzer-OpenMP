#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utilspar.h"
#include "statisticspar.h"


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

/*
 * Return a matrix to store the measures of min, max, median, mean and standard
 * deviation by region. Each row is a measure, each column is a region.
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
 * The following functions will fill the regions with taken the measures.
 * The convention to access the measures is measures[regionIndex][measureIndex][cityIndex].
 * This convention was adopted because it will facilitate the next step.
 * The next step is take the measures by region, so we'll need to sum up all the
 * cities from a region. So it's better having the cities sequencially in the
 * memory.
*/
void fill_min(Region* regions, Measures* measures, Input* input, int maxGrade)
{
	#pragma omp parallel for
	for(int i=0; i<input->nRegions; i++){
		#pragma omp parallel for
		for(int j=0; j<input->nCities; j++){
			measures->city[i][0][j] = find_min(regions[i][j], input->nStudents);
		}
		measures->region[0][i] = find_min_double(measures->city[i][0], input->nCities);
	}
	measures->country[0] = find_min_double(measures->region[0], input->nRegions); 
}

void fill_max(Region* regions, Measures* measures, Input* input, int maxGrade)
{
	#pragma omp parallel for
	for(int i=0; i<input->nRegions; i++){
		#pragma omp parallel for
		for(int j=0; j<input->nCities; j++){
			measures->city[i][1][j] = find_max(regions[i][j], input->nStudents);
		}
		measures->region[1][i] = find_max_double(measures->city[i][1], input->nCities);
	}
	measures->country[1] = find_max_double(measures->region[1], input->nRegions); 
}

void fill_median(Region* regions, Measures* measures, Input* input, int maxGrade)
{
	#pragma omp parallel sections
	{
		// median by cities
		#pragma omp section
		{
			#pragma omp parallel for
			for(int i=0; i<input->nRegions; i++){
				#pragma omp parallel for
				for(int j=0; j<input->nCities; j++){
					measures->city[i][2][j] = find_median(regions[i][j], input->nStudents, maxGrade+1);
				}
			}
		}
		// median by regions
		#pragma omp section
		{
			#pragma omp parallel for
			for(int i=0; i<input->nRegions; i++){
				measures->region[2][i] = find_median(regions[i][0], (input->nCities) * (input->nStudents), maxGrade+1);
			}
		}
		// median by country
		#pragma omp section
		{
			measures->country[2] = find_median_country(regions, (input->nRegions), (input->nCities), (input->nStudents), maxGrade+1);
		}
	}
}

// Get the region that has the best average
int getBestRegion(double** measuresByRegion){
	return find_pos_of_max_double(measuresByRegion[3], measuresByRegion[4] - measuresByRegion[3]);
}

int getBestCity(double*** measuresByCity, int nRegions, int nCities){
	int max_city = find_pos_of_max_double(measuresByCity[0][3], nCities);
	double max_val = measuresByCity[0][3][max_city];
	for(int i = 1; i< nRegions; i++){
		int tmp = find_pos_of_max_double(measuresByCity[i][3], nCities);
		if(max_val < measuresByCity[i][3][tmp]){
			max_val = measuresByCity[i][3][tmp];
			max_city = i * nCities + tmp;
		}
	}
	return max_city;
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

//Fill standard deviation by city and Region
void fillStddevByCityAndRegion(Region *regions, Measures* measures , Input *input){
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            #pragma omp parallel for
            for(int i = 0 ; i < input->nRegions ; i++){
                for (int j = 0; j < input->nCities ; j++){
                    measures->city[i][4][j] = calculate_stddev(regions[i][j], input->nStudents);
                }
            }
        }
        #pragma omp section
        {
            #pragma omp parallel for
            for(int j = 0 ; j < input->nRegions; j++){
                measures->region[4][j] = calculate_stddev(regions[j][0], (input->nCities) * (input->nStudents));
            }
        }
    }
}

//Fill standard deviation by country
void fillStddevByCountry(Region *regions, Measures *measures , Input *input){
    measures->country[4] = calculate_stddev_country(regions , input->nRegions, input->nCities , input->nStudents);
}

//Fill average by city, Region, and Country
void fillAvgByCityRegionCountry(Region *regions , Measures *measures, Input *input){
    #pragma parallel for
    for(int i = 0 ; i < input->nRegions ; i++){
        #pragma parallel for
        for(int j = 0 ; j < input->nCities ; j++){
            measures->city[i][3][j] = calculate_average(regions[i][j], input->nStudents);
        }
        measures->region[3][i] = calculate_average_double(measures->city[i][3] , input->nCities);
    }
    measures->country[3] = calculate_average_double(measures->region[3], input->nRegions);
}
