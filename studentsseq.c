#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utils.h"

#define FILENAME "input.in"

// This typedef was created in order to simplify syntax
typedef int** Region;


//Calculates the maximum grade of a certain city
int find_max(int* grades, int nStudents){
	int mx = grades[0];
	for(int i=1;i<nStudents;i++){
		if(mx < grades[i]) mx = grades[i];
	}
	return mx;
}

//calculates the minimum grade of a city
int find_min(int* grades, int nStudents){
	int mn = grades[0];
	for(int i = 1; i<nStudents; i++){
		if(mn > grades[i]) mn = grades[i];
	}
	return mn;
}

int main(){
    // Read input
    Input input;
    if(!readInput(FILENAME, &input)){
        exit(1);
    }

    // Create all matrices
    srand((unsigned) input.seed);
    // Array of regions, where a region is basically a matrix
    Region* regions = (Region *) malloc(sizeof(Region) * input.nRegions);
    int i, j, k;
    // For each region
    for(i=0; i<input.nRegions; i++){
        // Allocate a region
        regions[i] = matrix_new(input.nCities, input.nStudents);

        // Fill matrix
        for(j=0; j<input.nCities; j++){
            for(k=0; k<input.nStudents; k++){
                regions[i][j][k] = rand()%101;
            }
        }
    }


    // TODO: Calculation, storage and printing

	for(i = 0; i<input.nRegions; i++){
		for(j = 0; j<input.nCities; j++){
			printf("%d, %d\n",find_min(regions[i][j],input.nStudents),find_max(regions[i][j],input.nStudents));
		}
		printf("\n");
	}




    return 0;
}
