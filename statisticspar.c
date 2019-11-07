#include<omp.h>
#include "statisticspar.h"

/*
 * Trabalho desenvolvido por:
 * Bruno Piazera Larsen, 9283872
 * Flavio Vinicius Vieira Santana, 9866552
 * Lucas Nobuyuki Takahashi, 10295670
 * Mateus Castilho Leite, 9771550
 * Vinicius Henrique Borges, 9771546
 *
 *
 * PCAM:
 * Particionamento:
 * Será feito um particionamento fino, por função e por dados. Haverão R*C*A*5
 * tarefas para o calculo de medidas por cidade, onde R é o número de Regiões,
 * C é o número de cidades por região e A é o número de alunos por cidade e 5
 * o número de medidas a serem calculadas.  * Livremente falando, cada uma
 * dessas tarefas contribuirão com uma nota de aluno * para o cálculo da
 * medida a que lhes foi atribuída.
 * Analogamente parte das tarefas serão reaproveitadas para o cálculo das medidas
 * maior, menor, média aritmética, por região e por país, nas etapas seguintes. Para
 * o cálculo de mediana e desvio padrão e por pais serão geradas tarefas que poderam
 * trabalhar em paralelo desde do inicio do calculo de medidas.
 * Também haverá uma tarefa que imprimirá todos os resultados na tela.
 *
 * Comunicação:
 * Aqui é necessário analisar as dependências entre os dados:
 * média aritmética cidades <- média aritmética região <- média aritmética país
 * maior valor cidades <- maior valor região <- maior valor país
 * menor valor cidades <- menor valor região <- menor valor país
 * mediana cidada
 * mediana região
 * mediana país
 * desvio cidada
 * desvio região
 * desvio país
 * coeficiente de variância cidada
 * coeficiente de variância região
 * coeficiente de variância país
 *
 * Onde A <- B indica que a operação B depende o resultado da operação A.
 * Assim podemos executar as medianas, desvios e coeficientes podem ser executadas em paralelo
 * porém a média,maior e menor são dependentes e criam uma fila onde o resultado de um é passado para o proximo.
 *
 * Quando uma medida X for calculada para as cidades, regiões ou país o resultado
 * será passado para a tarefa que imprime os resultados.
 *
 * Quanto à parte mais fina do particionamento, as m tarefas de uma coluna de M
 * conversarão entre si para o cálculo da medida. Por exemplo, para o cálculo da
 * média para a primeira coluna de M, os elementos da coluna farão uma soma em
 * árvore e o resultado dessa soma será dividido pelo número de elementos por
 * uma das threads. O mesmo tipo de otimização será feito para as outras
 * medidas.
 *
 * Aglomeração:
 * Aglomeraremos as tarefas da seguinte forma:
 * as tarefas de calculo da média da cidade, maior da cidade, menor da cidade,
 * as medianas, desvios e coeficientes podem cada um ser uma tarefa que podem ser
 * executadas de forma idependente.
 * Já a média de região, maior de região, menor de região seram cada uma tarefa que
 * depende do resultado das tarefas média,maior,menor da cidade, e podem ser executados
 * concorrentemente.
 * As 3 ultimas tarefas, média do país, maior do país, menor do país serão feitas em
 * paralelo entre si.
 * Quando essas tarefas acabam além de enviar os dados para as tarefas que necessitam
 * dos dados elas também vão enviar o resultado para a tarefa que vai imprimir o
 * resultado.
 *
 * Mapeamento:
 * O mapeamento vai ser feito x threads, sendo x a quantidade de processadores na maquina
 * para então distribuir nela as tarefas que devem ser feitas. Respeitando a ordem da
 * comunicação entre os processos
 *
 */

/*
 * Calculates the minimum grade of a certain city
 *
 * The algorithm used iterates through the array, storing the current minimum value untill the end of the array, and returning this
 *
 */
int find_min(int* grades, int nStudents){
	int mn = grades[0];
    #pragma omp parallel for reduction(min: mn)
	for(int i = 1; i<nStudents; i++){
		if(mn > grades[i]) mn = grades[i];
	}
	return mn;
}

/*
 * Calculates the maximum grade of a certain city
 *
 * The algorithm used iterates through the array, storing the current maximum value untill the end of the array, and returning this
 *
 */
int find_max(int* grades, int nStudents){
	int mx = grades[0];
    #pragma omp parallel for reduction(max: mx)
	for(int i=1;i<nStudents;i++){
		if(mx < grades[i]) mx = grades[i];
	}
	return mx;
}

// Calculate the position of the maximum value of an array
int find_pos_of_max_double(double* vet, int size){
	double max = vet[0];
	int pos = 0;
	int i;
	for(i=1; i<size; i++){
		if(max < vet[i]){
			max = vet[i];
			pos = i;
		}
	}
	return pos;
}

/*
 * Calculates the median grade of a certain city
 *
 * The algorithm is divided into 2 parts. Firstly it runs the first half of a radix sort, filling the buckets; Then it counts how many grades
 * are equal to a certain amount and once this count reaches half of the vector, the median is found.
 *
 * The function returns double in case the amount of students is even, in which case an average will be returned.
 */
double find_median(int* grades, int nStudents, int range){
	//Instead of sorting the array, we count how many of each grade are there, and parsing that array instead;
	//Making the complexity be O(n) instead of O(n*log(n))
	int sorted[range];
	int count = 0,i, index = nStudents/2; //the index of the median, should nStudents be odd
	double add;
	for(i=0;i<range;i++){
		sorted[i] = 0;
	}
	for(i = 0; i < nStudents; i++){
		sorted[grades[i]] ++;//counts how many of a given grade there are.
	}

	i = 0;
	if(nStudents % 2){
		//If there's an odd amount of students, the median is trivial to calculate
		index ++; //increases one because once count reaches the index, we need the next occurence
		while(i < range && count + sorted[i] < index){ //while the amount of grades has not superseeded the index of the median, keep going on
			count += sorted[i];
			i++;
		}
		//once the loop has been exited, the amount of indexes passed is equal to or greater than the median, therefore, the i-th grade has happened in the median index
		return i;
	}
	else{
		//if there's an even amount of students, the median needs an extra step
		//Once again, we wait until count and sorted superseeded the index of the median
		while(i < range && count + sorted[i] < index){//max grade is here just as sanity check
			count += sorted[i];
			i++;
		}
		if(i < range && count + sorted[i] > index){//if the new value goes over the index, we know that the 2 values are the same, we can return i
			return i;
		}
		//otherwise, the first value is the current i, and the next value is the first bucket greater than 0
		add = i++;//storing first value and advancing the counter
		while(i < range && !sorted[i++]);//looks for the first non-zero value
		//once found, i is one over the first non-zero value, so we must decrement it
		i--;
		return (add + i)/2; //and returns the average
	}
}

/*
 * Calculates the average grade of the students of the city
 *
 * The algorithm adds up all values of the array and then divides it by the amount of students in the city
 */
double calculate_average(int* grades, int nStudents){
	double sum = 0;
    #pragma omp parallel for reduction(+: sum)
	for(int i = 0; i < nStudents; i++){
		sum += grades[i];
	}
	return sum/nStudents;
}

/*
 * Calculates standard deviation of the grades of the students of a given city
 *
 * The algorithm calculates the average first, then the standard deviation
 *
 * The average is recalculated to avoid needing to wait for average calculations, should this code be used directly for paralelization
 */
double calculate_stddev(int* grades, int nStudents){
	double sum = 0, avg = 0;
	if(nStudents <= 0) return 0; //there's no standard deviation if there's a single value. Also avoids zero division error
	#pragma omp parallel for reduction(+: avg)
    for(int i = 0; i<nStudents; i++){
		avg += grades[i];
	}
	avg /= nStudents;
    #pragma omp parallel for reduction(+: sum)
	for(int i = 0; i < nStudents; i++){
		sum += (grades[i] - avg)*(grades[i] - avg);
	}
	sum /= (nStudents - 1);
	return sqrt(sum);
}

/*
 * Calculates the minimum grade of a certain city
 *
 * The algorithm used iterates through the array, storing the current minimum value untill the end of the array, and returning this
 *
 */
double find_min_double(double* grades, int nStudents){
	double mn = grades[0];
    #pragma omp parallel for reduction(min: mn)
	for(int i = 1; i<nStudents; i++){
		if(mn > grades[i]) mn = grades[i];
	}
	return mn;
}

/*
 * Calculates the maximum grade of a certain city
 *
 * The algorithm used iterates through the array, storing the current maximum value untill the end of the array, and returning this
 *
 */
double find_max_double(double* grades, int nStudents){
	double mx = grades[0];
    #pragma omp parallel for reduction(max: mx)
	for(int i=1;i<nStudents;i++){
		if(mx < grades[i]) mx = grades[i];
	}
	return mx;
}

/*
 * Calculates the median grade of a certain city
 *
 * The algorithm is divided into 2 parts. Firstly it runs the first half of a radix sort, filling the buckets; Then it counts how many grades
 * are equal to a certain amount and once this count reaches half of the vector, the median is found.
 *
 * The function returns double in case the amount of students is even, in which case an average will be returned.
 */
double find_median_country(int*** grades,int nRegions, int nCities, int nStudents, int range){
	//Instead of sorting the array, we count how many of each grade are there, and parsing that array instead;
	//Making the complexity be O(n) instead of O(n*log(n))
	int sorted[range];
	int count = 0, i, j, k, index = (nRegions * nCities * nStudents)/2; //the index of the median, should nStudents be odd
	double add;
	for(i=0;i<range;i++){
		sorted[i] = 0;
	}
	for(i = 0; i < nRegions; i++){
		for(j = 0; j < nCities; j++){
			for(k = 0; k < nStudents; k++){
				sorted[grades[i][j][k]] ++;//counts how many of a given grade there are.
			}
		}
	}

	i = 0;
	if(nStudents % 2){
		//If there's an odd amount of students, the median is trivial to calculate
		index ++; //increases one because once count reaches the index, we need the next occurence
		while(i < range && count + sorted[i] < index){ //while the amount of grades has not superseeded the index of the median, keep going on
			count += sorted[i];
			i++;
		}
		//once the loop has been exited, the amount of indexes passed is equal to or greater than the median, therefore, the i-th grade has happened in the median index
		return i;
	}
	else{
		//if there's an even amount of students, the median needs an extra step
		//Once again, we have index 1 over the real value, because we need the next occurence.
		while(i < range && count + sorted[i] < index){//max grade is here just as sanity check
			count += sorted[i];
			i++;
		}
		if(i < range && count + sorted[i] > index){//if the new value goes over the index, we know that the 2 values are the same, we can return i
			return i;
		}
		//otherwise, the first value is the current i, and the next value is the first bucket greater than 0
		add = i++;//storing first value and advancing the counter
		while(i < range && !sorted[i++]);//looks for the first non-zero value
		//i is one over the desired value
		i--;
		return (add + i)/2; //and returns the average
	}
}

/*
 * Calculates the average grade of the students of the city
 *
 * The algorithm adds up all values of the array and then divides it by the amount of students in the city
 */
double calculate_average_double(double* grades, int nStudents){
	double sum = 0;
    #pragma omp parallel for reduction (+: sum)
	for(int i = 0; i < nStudents; i++){
		sum += grades[i];
	}
	return sum/nStudents;
}

/*
 * Calculates standard deviation of the grades of the students of a given city
 *
 * The algorithm calculates the average first, then the standard deviation
 *
 * The average is recalculated to avoid needing to wait for average calculations, should this code be used directly for paralelization
 */
double calculate_stddev_country(int*** grades, int nRegions, int nCities, int nStudents){
	if(nStudents <= 1) return 0; //there's no standard deviation if there's a single value. Also avoids zero division error
    double sum = 0, avg = 0;
	#pragma omp parallel for reduction(+: avg)
    for(int i = 0; i<nRegions; i++){
        #pragma omp parallel for reduction(+: avg)
        for(int j = 0; j<nCities; j++){
            #pragma omp parallel for reduction(+: avg)
            for(int k = 0; k<nStudents; k++){
                avg += grades[i][j][k];
            }
        }
    }
	avg /= (nRegions * nCities * nStudents);
    #pragma omp parallel for reduction (+: sum)
	for(int i = 0; i < nRegions; i++){
        #pragma omp parallel for reduction (+: sum)
		for(int j = 0; j < nCities; j++){
            #pragma omp parallel for reduction (+: sum)
			for(int k = 0; k < nStudents; k++){
				sum += (grades[i][j][k] - avg)*(grades[i][j][k] - avg);
			}
		}
	}
	sum /= (nRegions * nCities * nStudents - 1);
	return sqrt(sum);
}
