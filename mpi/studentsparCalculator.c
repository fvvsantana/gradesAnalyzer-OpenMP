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

	printf("%s tem o processo %d\n" , hostName , rank);

	int receivInput[4];

	// recebe os dados do input e a quantidade de processos inicializados
	MPI_Bcast(receivInput , 4 , MPI_INT , 0 , parentComm);
	input.nCities = receivInput[0];
	input.nRegions = receivInput[1];
	input.nStudents = receivInput[2];
	int processInit = receivInput[3];


	// calcula quantas regiões vai receber
	int amountRegionsReceive =  input.nRegions / processInit + (input.nRegions % processInit  > rank);
	printf("processo %d tem que receber %d\n" , rank , amountRegionsReceive);
	// testa para ver se tem alguma região para receber
	if(amountRegionsReceive){
		// aloca a estrutura que vai armazenar as regiões
		Region *regions = malloc(sizeof(Region) * amountRegionsReceive);
		// recebe cada região de forma não bloqueante para já ir ajeitando os ponteiros das cidades.
		for(int i  = 0 ; i < amountRegionsReceive ; i++){
			MPI_Request request;
			MPI_Status status;
			regions[i] = (int**) malloc(sizeof(int) * input.nStudents * input.nCities);
			printf("receive não bloqueante com no processo %d do remente %d com tag %d\n" , rank , 0 , i);
			int nada[2];
			MPI_Irecv(nada , 2 , MPI_INT , 0 , i , parentComm, &request);
			for(int j = 1 ; j < input.nCities ; j++){
				regions[i][j] = regions[i][j-1] + input.nStudents;
			}
			printf("arrumou os ponteiros\n");
			MPI_Wait(&request , &status);
			if(status.MPI_ERROR == MPI_SUCCESS){
				int contador;
				MPI_Get_count(&status , MPI_INT, &contador);
				printf("a quantidade de dados recebido foi: %d %d %d\n" , contador , nada[0] , nada[1]);
			}
		}

		//char aux[10000];

		for(int i = 0 ; i < amountRegionsReceive ; i++){
			for(int j = 0 ; j < input.nCities ; j++){
				//int auxPosi = sprintf(aux , "%d %d %d " , rank , i , j);
				printf("%d %d %d " , rank , i , j);
				fflush(stdout);
				for(int z = 0 ; z < input.nStudents ; z++){
					//auxPosi += sprintf(aux + auxPosi , "%d " , regions[i][j][z]);
					printf("%d " , regions[i][j][z]);
				}
				//aux[auxPosi] = 0;
				//printf("%s\n" , aux);
			}
		}




		for(int i = 0 ; i < amountRegionsReceive ; i++){
			free(regions[i]);
		}
		free(regions);
	}





	MPI_Finalize();
	return 0;
}
