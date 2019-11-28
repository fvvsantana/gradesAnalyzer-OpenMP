#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "utilspar.h"
#include "statisticspar.h"
#include <mpi.h>


int main (int argc , char* argv[]){
	int rank;
	Input input;
	char hostName[7];
	int hostNameLen;

	MPI_Init(&argc , &argv);

	MPI_Comm parentComm;

	// get rank of process.
	MPI_Comm_rank(MPI_COMM_WORLD , &rank);
	MPI_Comm_get_parent(&parentComm);
	MPI_Get_processor_name(hostName , &hostNameLen);

	int receivInput[4];

	// recebe os dados do input e a quantidade de processos inicializados
	MPI_Bcast(receivInput , 4 , MPI_INT , 0 , parentComm);
	input.nCities = receivInput[0];
	input.nStudents = receivInput[2];
	int processInit = receivInput[3];


	// calcula quantas regiões vai receber
	int amountRegionsReceive =  receivInput[1] / processInit + (receivInput[1] % processInit  > rank);
	input.nRegions = amountRegionsReceive;
	printf("processo %d tem que receber %d\n" , rank , amountRegionsReceive);
	// testa para ver se tem alguma região para receber
	if(input.nRegions){
		//aloca a medidas
		Measures measures;
		measures.city = allocateForMeasuresByCity(&input, NMEASURES);
		measures.region = allocateForMeasuresByRegion(&input, NMEASURES);
		measures.country = allocateForMeasuresByCountry(NMEASURES);

		// aloca a estrutura que vai armazenar as regiões
		Region *regions = malloc(sizeof(Region) * amountRegionsReceive);
		// recebe cada região de forma não bloqueante para já ir ajeitando os ponteiros das cidades.
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

		for(int i = 0 ; i < input.nRegions ; i++){
			for(int j = 0 ; j < input.nCities ; j++){
				printf("%d %d %d " , rank , i , j);
				fflush(stdout);
				for(int z = 0 ; z < input.nStudents ; z++){
					printf("%d " , regions[i][j][z]);
				}
				printf("\n" );
			}
			printf("\n");
		}




		for(int i = 0 ; i < amountRegionsReceive ; i++){
			free(regions[i][0]);
			free(regions[i]);
		}
		free(regions);
	}





	MPI_Finalize();
	return 0;
}
