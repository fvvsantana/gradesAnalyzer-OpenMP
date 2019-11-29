#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "utilspar.h"
#include "statisticspar.h"
#include <mpi.h>

#define MAX_GRADE 100

#define N_PROCESS 11

void debugPrintRegions(Input* input, Region* regions);

int main(int argc , char *argv[]){
	// Read input
	Input input;
	readInput(&input);
	int rank;

	MPI_Comm interCommmunicator;

	int err_code[N_PROCESS];
	int amountRegionsPerProcess[N_PROCESS];

	MPI_Init(&argc , &argv);

	// Allocation makes sure that each region has contiguous data, but he regions themselves might not be.
	// This is done because the chance of a failure with a very big malloc is much greater
	// if we had to account for alloc fail, we'd have to code the whole program twice, so we decided with the easiest implementation
	Region* regions = generateRegions(&input, MAX_GRADE + 1);
	Measures measures;
	measures.city = allocateForMeasuresByCity(&input, NMEASURES);
	measures.region = allocateForMeasuresByRegion(&input, NMEASURES);
	measures.country = allocateForMeasuresByCountry(NMEASURES);

	// Variables to store best region and best city
	int bestRegion, bestCity;

	//enable nested parallel
	omp_set_nested(1);

	// create process to execute programn
	//
	int processInit = N_PROCESS;

	MPI_Comm_rank(MPI_COMM_WORLD , &rank);
	printf(" %d %d  %d\n" , input.nRegions , input.nCities , input.nStudents);

	MPI_Comm_spawn("studentsparCalculator" , MPI_ARGV_NULL , N_PROCESS , MPI_INFO_NULL , rank , MPI_COMM_WORLD , &interCommmunicator , err_code);


	//error checking
	for(int i = 0 ; i < N_PROCESS ; i++){
		if(err_code[i] != MPI_SUCCESS){
			printf("Não consegui inicializar o processo %d\n" , i);
			processInit--;
		}
	}

	// starts the time measurement
	double begin = omp_get_wtime();

	int bufferSendInput[4] = { input.nCities , input.nRegions , input.nStudents  , processInit};
	MPI_Bcast(bufferSendInput , 4 , MPI_INT , MPI_ROOT , interCommmunicator);
	int amountOfRegions = input.nRegions / processInit;
	int restRegions = input.nRegions % processInit;
	int contRegions = 0;
	MPI_Request request;
	for(int i = 0 ; i < processInit ; i++){
		// amount of regions to send to process 'i'
		amountRegionsPerProcess[i] = amountOfRegions + (restRegions > 0);
		restRegions--;
		printf("processo %d vai receber %d regios\n" , i , amountRegionsPerProcess[i]);
		// if there's no more regions to be sent, do nothing
		if(amountRegionsPerProcess[i]){
			for(int j = 0 ; j < amountRegionsPerProcess[i] ; j++){
				printf("send não bloqueante para %d com tag %d\n" , i , j);
				// Each region is sent separetly because they are not contiguous in memory.
				MPI_Send(regions[contRegions][0] , input.nCities * input.nStudents , MPI_INT , i , j , interCommmunicator );
				contRegions++;
			}
		}
	}



    // tempo ocioso se alguem conseguir colocar algo aqui, mas que termine rapido o bastante para começar a receber os dados


	// Starts receiving data
	int dataRecvGather[processInit];
	int displ[processInit];
	for(int i = 0 ; i < processInit ; i++){
		dataRecvGather[i] = amountRegionsPerProcess[i] * NMEASURES;
		displ[i] = 0;
	}
	//MPI_Gatherv(NULL , 0 , MPI_DOUBLE , measures.region ,dataRecvGather, displ, MPI_DOUBLE , MPI_ROOT , interCommmunicator);

    // Paralelize data receiving and post-processing
    #pragma omp parallel sections shared(measures)
    {
        #pragma omp section
        {
            int cont = 0;
            for(int i = 0 ; i < processInit ; i++){
                for(int j = 0 ; j < amountRegionsPerProcess[i] ; j++){
                    MPI_Status status;
                    //MPI_Recv(measures.city[cont][0] , input.nCities * NMEASURES , MPI_DOUBLE , i , j , interCommmunicator , &status);
                    cont++;
                }
            }
        }
    }




	// finish time measurement
	double end = omp_get_wtime();
	// Calculate time spent
	double timeSpent = end - begin;

	// Printing
/*	printMeasuresByCity(measures.city, &input);
	printMeasuresByRegion(measures.region, &input);
	printMeasuresByCountry(measures.country);
	printf("\nMelhor regiao: Regiao %d\n", bestRegion);
	printf("Melhor cidade: Regiao %d, Cidade %d\n", bestCity/input.nCities, bestCity % input.nCities);
	printf("Time spent: %lf seconds\n", timeSpent);*/


	// Free array of regions
	freeRegions(regions, input.nRegions);
	// Free array of regions for result
	freeMeasuresByCity(measures.city, input.nRegions);
	// Free matrix of measures by region
	matrix_delete_double(measures.region);
	// Free array of measures by country
	free(measures.country);

	MPI_Finalize();

	return 0;
}

// Print the randomly generated regions
void debugPrintRegions(Input* input, Region* regions){
	int i;
	// Show matrices
	for(i=0; i<input->nRegions; i++){
		matrix_print(regions[i], input->nCities, input->nStudents);
		printf("\n");
	}
}
