#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "utilspar.h"
#include "statisticspar.h"

#define MAX_GRADE 100
#define NMEASURES 5

void debugPrintRegions(Input* input, Region* regions);

int main(){
	// Read input
	Input input;
	readInput(&input);

	// Allocation
	Region* regions = generateRegions(&input, MAX_GRADE + 1);
	Measures measures;
	measures.city = allocateForMeasuresByCity(&input, NMEASURES);
	measures.region = allocateForMeasuresByRegion(&input, NMEASURES);
	measures.country = allocateForMeasuresByCountry(NMEASURES);

    //enable nested parallel
    omp_set_nested(1);

	// Get time
	double begin = omp_get_wtime();

	// Taking measures
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			fill_median(regions, &measures, &input, MAX_GRADE);
		}
		#pragma omp section
		{
		    fillAvgStdDedByCityRegionCountry(regions , &measures , &input);
		}
        #pragma omp section
        {
			fill_max(regions, &measures, &input, MAX_GRADE);
		    fill_min(regions, &measures, &input, MAX_GRADE);
        }
	}

	int bestRegion, bestCity;
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			bestRegion = getBestRegion(measures.region);
		}
		#pragma omp section
		{
			bestCity = getBestCity(measures.city, input.nRegions, input.nCities);
		}
	}

	// Get time
	double end = omp_get_wtime();
	// Calculate time spent
	double timeSpent = end - begin;

	// Printing
	debugPrintRegions(&input, regions);
	printMeasuresByCity(measures.city, &input);
	printMeasuresByRegion(measures.region, &input);
	printMeasuresByCountry(measures.country);
	printf("\nMelhor regiao: Regiao %d\n", bestRegion);
	printf("Melhor cidade: Regiao %d, Cidade %d\n", bestCity/input.nCities, bestCity % input.nCities);
	printf("Time spent: %lf seconds\n", timeSpent);


	// Free array of regions
	freeRegions(regions, input.nRegions);
	// Free array of regions for result
	freeMeasuresByCity(measures.city, input.nRegions);
	// Free matrix of measures by region
	matrix_delete_double(measures.region);
	// Free array of measures by country
	free(measures.country);

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
