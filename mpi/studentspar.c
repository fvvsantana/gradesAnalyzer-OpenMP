#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "utilspar.h"
#include "statisticspar.h"
#include <mpi.h>

#define MAX_GRADE 100
#define NMEASURES 5

#define N_PROCESS 11

void debugPrintRegions(Input* input, Region* regions);

int main(int argc , char *argv[]){
	// Read input
	Input input;
	readInput(&input);
	int rank;

	MPI_Comm interCommmunicator;

	int err_code[N_PROCESS];

	MPI_Init(&argc , &argv);

	// Allocation
	Region* regions = generateRegions(&input, MAX_GRADE + 1);
	Measures measures;
	measures.city = allocateForMeasuresByCity(&input, NMEASURES);
	measures.region = allocateForMeasuresByRegion(&input, NMEASURES);
	measures.country = allocateForMeasuresByCountry(NMEASURES);

	// Variables to store best regio and best city
	int bestRegion, bestCity;

    //enable nested parallel
    omp_set_nested(1);

	// create process to execute programn
	//
	int processInit = N_PROCESS;

	MPI_Comm_rank(MPI_COMM_WORLD , &rank);
	printf(" %d %d  %d\n" , input.nRegions , input.nCities , input.nStudents);

	printf("inicialização dos processo\n");
	MPI_Comm_spawn("studentsparCalculator" , MPI_ARGV_NULL , N_PROCESS , MPI_INFO_NULL , rank , MPI_COMM_WORLD , &interCommmunicator , err_code);


	printf("teste de quem iniciou\n");
	for(int i = 0 ; i < N_PROCESS ; i++){
		if(err_code[i] != MPI_SUCCESS){
			printf("Não consegui inicializar o processo %d\n" , i);
			processInit--;
		}
	}
	printf("fim da inicialização dos %d processos \n" , processInit);

	printf("print dos dados\n");
	debugPrintRegions(&input , regions);

	// Get time
	double begin = omp_get_wtime();

	int bufferSendInput[4] = { input.nCities , input.nRegions , input.nStudents  , processInit};
	MPI_Bcast(bufferSendInput , 4 , MPI_INT , MPI_ROOT , interCommmunicator);
	int amountOfRegions = input.nRegions / processInit;
	int restRegions = input.nRegions % processInit;
	int contRegions = 0;
	MPI_Request request;
	for(int i = 0 ; i < processInit ; i++){
		// amount of regions to send to process 'i'
		int aux = amountOfRegions + (restRegions > 0);
		restRegions--;
		printf("processo %d vai receber %d regios\n" , i , aux);
		// don't send anything if aux is 0
		if(aux){
			for(int j = 0 ; j < aux ; j++){
				printf("send não bloqueante para %d com tag %d\n" , i , j);
				// Non-Clock send regions one a one because ... escrever aqui a explicação, não sou muito bom com ingles
				MPI_Send(regions[contRegions][0] , input.nCities * input.nStudents , MPI_INT , i , j , interCommmunicator );
				contRegions++;
			}
		}
	}

	// Taking measures

	// Get time
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

// Print randomly generated regions
void debugPrintRegions(Input* input, Region* regions){
	int i;
	// Show matrices
	for(i=0; i<input->nRegions; i++){
		matrix_print(regions[i], input->nCities, input->nStudents);
		printf("\n");
	}
}
