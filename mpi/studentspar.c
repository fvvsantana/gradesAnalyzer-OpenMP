#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "utilspar.h"
#include "statisticspar.h"
#include <mpi.h>

#define MAX_GRADE 100

#define N_PROCESS 11

//void debugPrintRegions(Input* input, Region* regions);

int main(int argc , char *argv[]){
	// Read input
	Input input;
	// Put read arguments inside structure input
	readInput(&input);
	int rank;

	MPI_Comm interCommmunicator;

	int err_code[N_PROCESS];
	int amountRegionsPerProcess[N_PROCESS];

	MPI_Init(&argc , &argv);

	// Allocation
	// Allocate and fill regions with random values
	Region* regions = generateRegions(&input, MAX_GRADE + 1);
	Measures measures;
	// Allocate structures to store measures
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
	//printf(" %d %d  %d\n" , input.nRegions , input.nCities , input.nStudents);

	MPI_Comm_spawn("studentsparCalculator" , MPI_ARGV_NULL , N_PROCESS , MPI_INFO_NULL , rank , MPI_COMM_WORLD , &interCommmunicator , err_code);


	for(int i = 0 ; i < N_PROCESS ; i++){
		if(err_code[i] != MPI_SUCCESS){
			printf("Não consegui inicializar o processo %d\n" , i);
			processInit--;
		}
	}

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
		amountRegionsPerProcess[i] = amountOfRegions + (restRegions > 0);
		restRegions--;
		//printf("processo %d vai receber %d regios\n" , i , amountRegionsPerProcess[i]);
		// don't send anything if aux is 0
		if(amountRegionsPerProcess[i]){
			for(int j = 0 ; j < amountRegionsPerProcess[i] ; j++){
				//printf("send não bloqueante para %d com tag %d\n" , i , j);
				// Non-Clock send regions one a one because ... escrever aqui a explicação, não sou muito bom com ingles
				MPI_Send(regions[contRegions][0] , input.nCities * input.nStudents , MPI_INT , i , j , interCommmunicator );
				contRegions++;
			}
		}
	}



    // tempo ocioso se alguem conseguir colocar algo aqui, mas que termine rapido o bastante para começar a receber os dados


	// aqui ele recebe o resultado dos calculos das regioes
	int dataRecvGather[processInit];
	int displ[processInit];
	dataRecvGather[0]  = amountRegionsPerProcess[0] * NMEASURES;
	displ[0] = 0;
	for(int i = 1 ; i < processInit ; i++){
		dataRecvGather[i] = amountRegionsPerProcess[i] * NMEASURES;
		displ[i] = displ[i-1] + amountRegionsPerProcess[i-1];
	}
	//MPI_Gatherv(NULL , 0 , MPI_DOUBLE , measures.region[0] ,dataRecvGather, displ, MPI_DOUBLE , MPI_ROOT , interCommmunicator);

    // faz tudo em paralelo enquanto recupera os resultados das cidades.
    #pragma omp parallel sections shared(measures)
    {
        #pragma omp section
        {
            int cont = 0;
            for(int i = 0 ; i < processInit ; i++){
                for(int j = 0 ; j < amountRegionsPerProcess[i] ; j++){
                    MPI_Status status;
                    MPI_Recv(measures.city[cont][0] , input.nCities * NMEASURES , MPI_DOUBLE , i , j , interCommmunicator , &status);
                    cont++;
                }
            }
        }
    }

	// Taking measures for country
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			fill_country_median(regions, &measures, &input, MAX_GRADE);
		}
		#pragma omp section
		{
		    fill_country_avg_std_dev(regions , &measures , &input);
		}
        #pragma omp section
        {
		    fill_country_min(&measures, &input);
			fill_country_max(&measures, &input);
        }
	}

	// Determine the best region and city
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
	printMeasuresByCity(measures.city, &input);
	printMeasuresByRegion(measures.region, &input);
	printMeasuresByCountry(measures.country);
	printf("\nMelhor regiao: Regiao %d\n", bestRegion);
	printf("Melhor cidade: Regiao %d, Cidade %d\n", bestCity/input.nCities, bestCity % input.nCities);
	printf("Tempo de resposta sem considerar E/S, em segundos: %lf s\n", timeSpent);


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
