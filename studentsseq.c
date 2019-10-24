#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utils.h"

#define FILENAME "input.in"
#define MAX_GRADE 101

// This typedef was created in order to simplify syntax
typedef int** Region;


/*
 * Calculates the maximum grade of a certain city
 *
 * The algorithm used iterates through the array, storing the current maximum value untill the end of the array, and returning this
 *
 */
int find_max(int* grades, int nStudents){
	int mx = grades[0];
	for(int i=1;i<nStudents;i++){
		if(mx < grades[i]) mx = grades[i];
	}
	return mx;
}

/*
 * Calculates the minimum grade of a certain city
 *
 * The algorithm used iterates through the array, storing the current minimum value untill the end of the array, and returning this
 *
 */
int find_min(int* grades, int nStudents){
	int mn = grades[0];
	for(int i = 1; i<nStudents; i++){
		if(mn > grades[i]) mn = grades[i];
	}
	return mn;
}

/*
 * Calculates the median grade of a certain city
 *
 * The algorithm is divided into 2 parts. Firstly it runs the first half of a radix sort, filling the buckets; Then it counts how many grades 
 * are equal to a certain amount and once this count reaches half of the vector, the median is found.
 *
 * The function returns double in case the amount of students is even, in which case an average will be returned.
 */
double find_median(int* grades, int nStudents){
	//Instead of sorting the array, we count how many of each grade are there, and parsing that array instead;
	//Making the complexity be O(n) instead of O(n*log(n))
	int sorted[MAX_GRADE];
	int count = 0,i, index = nStudents/2; //the index of the median, should nStudents be odd
	double add;
	for(i=0;i<MAX_GRADE;i++){
		sorted[i] = 0;
	}
	for(i = 0; i < nStudents; i++){
		sorted[grades[i]] ++;//counts how many of a given grade there are.
	}

	i = 0;
	if(nStudents % 2){
		//If there's an odd amount of students, the median is trivial to calculate
		index ++; //increases one because once count reaches the index, we need the next occurence
		while(i < MAX_GRADE && count + sorted[i] < index){ //while the amount of grades has not superseeded the index of the median, keep going on
			count += sorted[i];
			i++;
		}
		//once the loop has been exited, the amount of indexes passed is equal to or greater than the median, therefore, the i-th grade has happened in the median index
		return i;
	}
	else{
		//if there's an even amount of students, the median needs an extra step
		//Once again, we have index 1 over the real value, because we need the next occurence.
		while(i < MAX_GRADE && count + sorted[i] < index){//max grade is here just as sanity check
			count += sorted[i];
			i++;
		}
		if(i < MAX_GRADE && count + sorted[i] > index){//if the new value goes over the index, we know that the 2 values are the same, we can return i
			return i;
		}
		//otherwise, the first value is the current i, and the next value is the first bucket greater than 0
		add = i++;//storing first value and advancing the counter
		while(i < MAX_GRADE && !sorted[i++]);//looks for the first non-zero value
		return (add + i)/2; //and returns the average
	}
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
                regions[i][j][k] = rand()%MAX_GRADE;
				printf("%d ",regions[i][j][k]);//usado para testes dos calculos. REMOVER
            }
			printf("\n");//usado para testes dos calculos. REMOVER
        }
		printf("\n"); //usado para testes dos calculos. REMOVER
    }


    // TODO: Calculation, storage and printing

	for(i = 0; i<input.nRegions; i++){
		for(j = 0; j<input.nCities; j++){
			printf("min: %d, max: %d, mediana: %.1lf\n",
					find_min(regions[i][j],input.nStudents),
					find_max(regions[i][j],input.nStudents), 
					find_median(regions[i][j],input.nStudents)
				  );
		}
		printf("\n");
	}




    return 0;
}
