#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include "utils.h"
#include "statisticsseq.h"

#define MAX_GRADE 100
#define NMEASURES 5

void debugPrintRegions(Input* input, Region* regions);

int main(){
	int i, j;
    // Read input
    Input input;
    readInput(&input);

	// Allocation
	Region* regions = generateRegions(&input, MAX_GRADE + 1);
	double*** measuresByCity = allocateForMeasuresByCity(&input, NMEASURES);
	double** measuresByRegion = allocateForMeasuresByRegion(&input, NMEASURES);
	double* measuresByCountry = allocateForMeasuresByCountry(NMEASURES);


	// Get time
	double begin = omp_get_wtime();
	// Taking measures
	fillMeasuresByCity(regions, measuresByCity, &input, MAX_GRADE);
	fillMeasuresByRegion(regions, measuresByCity, measuresByRegion, &input, MAX_GRADE);
	fillMeasuresByCountry(regions, measuresByRegion, measuresByCountry, &input, MAX_GRADE);
	int bestRegion = getBestRegion(measuresByRegion);
	int bestCity = getBestCity(measuresByCity, input.nRegions, input.nCities);
	// Get time
	double end = omp_get_wtime();
	// Calculate time spent
	double timeSpent = end - begin;


	// Printing
	debugPrintRegions(&input, regions);
	printMeasuresByCity(measuresByCity, &input);
	printMeasuresByRegion(measuresByRegion, &input);
	printMeasuresByCountry(measuresByCountry);
	printf("\nMelhor regiao: Regiao %d\n", bestRegion);
	printf("Melhor cidade: Regiao %d, Cidade %d\n", bestCity/input.nCities, bestCity % input.nCities);
	printf("Time spent: %lf seconds\n", timeSpent);


	// Free array of regions
	freeRegions(regions, input.nRegions);
	// Free array of regions for result
	freeMeasuresByCity(measuresByCity, input.nRegions);
	// Free matrix of measures by region
    matrix_delete_double(measuresByRegion);
	// Free array of measures by country
	free(measuresByCountry);

    return 0;
}

// Print randomly generated regions
void debugPrintRegions(Input* input, Region* regions){
	int i;
	// Show matrices
	for(i=0; i<input->nRegions; i++){
		matrix_print(regions[i], input->nCities, input->nStudents);
		printf("\n");
	}
}
