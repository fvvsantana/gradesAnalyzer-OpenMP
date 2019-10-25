#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "utils.h"
#include "statisticsseq.h"

#define MAX_GRADE 100


int main(){
    // Read input
    Input input;
    readInput(&input);

	Region* regions = generateRegions(&input, MAX_GRADE + 1);

	// Show matrices
	int i;
	for(i=0; i<input.nRegions; i++){
		matrix_print(regions[i], input.nCities, input.nStudents);
		printf("\n");
	}


	int j;
    // TODO: Calculation, storage and printing
	for(i = 0; i<input.nRegions; i++){
		for(j = 0; j<input.nCities; j++){
			printf("min: %d, max: %d, mediana: %.1lf, media: %.3lf, desvio padrao: %.3lf\n",
					find_min(regions[i][j],input.nStudents),
					find_max(regions[i][j],input.nStudents),
					find_median(regions[i][j],input.nStudents, 101),
					calculate_average(regions[i][j], input.nStudents),
					calculate_stddev(regions[i][j], input.nStudents)
				  );
		}
		printf("\n");
	}



	// Free data matrices
	for(i=0; i<input.nRegions; i++){
		matrix_delete(regions[i]);
	}
	// Free regions array
	free(regions);



    return 0;
}
