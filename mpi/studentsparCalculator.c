#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "utilspar.h"
#include "statisticspar.h"
#include <mpi.h>

#define MAX_GRADE 100

int main (int argc , char* argv[]){
	int rank;
	Input input;
	//char hostName[7];
	//int hostNameLen;

	MPI_Init(&argc , &argv);

	MPI_Comm parentComm;

	// get rank of process.
	MPI_Comm_rank(MPI_COMM_WORLD , &rank);
	MPI_Comm_get_parent(&parentComm);
	//MPI_Get_processor_name(hostName , &hostNameLen);

	int receivInput[4];

	// receive input data, and how many processes were initialized
	MPI_Bcast(receivInput , 4 , MPI_INT , 0 , parentComm);
	input.nCities = receivInput[0];
	input.nStudents = receivInput[2];
	int processInit = receivInput[3];


	// calculates how many regions each process receive
	// and makes input have the correct amount of regions
    
	// checks if there is a region to receive
	input.nRegions = receivInput[1] / processInit + ((receivInput[1] % processInit)  > rank);
	if(input.nRegions){
		//allocates the variables to store the results
		Measures measures;
		measures.city = allocateForMeasuresByCity(&input, NMEASURES);
		measures.region = allocateForMeasuresByRegion(&input, NMEASURES);
		measures.country = allocateForMeasuresByCountry(NMEASURES);

		// allocates the structure that will store the regions
		Region *regions = malloc(sizeof(Region) * input.nRegions);
		// receives each regions with a non-blocking functions and sets the city pointers
		for(int i  = 0 ; i < input.nRegions ; i++){
			MPI_Request request;
			MPI_Status status;
			regions[i] = (int**) malloc(sizeof(int*) * input.nCities);
			regions[i][0] = (int*) malloc(sizeof(int) * input.nCities *  input.nStudents);
			MPI_Irecv(regions[i][0] , input.nCities * input.nStudents , MPI_INT , 0 , i , parentComm, &request);
			for(int j = 1 ; j < input.nCities ; j++){
				regions[i][j] = regions[i][j-1] + input.nStudents;
			}
			MPI_Wait(&request , &status);
		}

        // calculates the measures here
        // ==================================
        #pragma omp parallel sections
		{
			#pragma omp section
			{
				fill_median(regions, &measures, &input, MAX_GRADE);
			}
			#pragma omp section
			{
				fill_avg_std_dev(regions , &measures , &input);
			}
			#pragma omp section
			{
				fill_min(regions, &measures, &input);
				fill_max(regions, &measures, &input);
			}
		}
        
        // ==================================

        // send results to the root process


        // send regions results
        MPI_Gatherv(measures.region[0] , input.nRegions * NMEASURES , MPI_DOUBLE , NULL ,NULL, NULL, MPI_DOUBLE , 0 , parentComm);

        for ( int i = 0 ; i < input.nRegions ; i++){
            MPI_Send(measures.city[i][0] , input.nCities * NMEASURES , MPI_DOUBLE , 0 , i , parentComm);
        }

        // Free array of regions
		for(int i = 0 ; i < input.nRegions ; i++){
			free(regions[i][0]);
			free(regions[i]);
		}
		free(regions);
        // Free array of regions for result
        freeMeasuresByCity(measures.city, input.nRegions);
        // Free matrix of measures by region
        matrix_delete_double(measures.region);

	}





	MPI_Finalize();
	return 0;
}
