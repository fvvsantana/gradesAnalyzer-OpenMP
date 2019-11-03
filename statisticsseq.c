#include "statisticsseq.h"

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

// Calculate the position of the maximum value of an array
int find_pos_of_max_double(double* vet, int size){
	int max = vet[0];
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
	for(int i = 0; i<nStudents; i++){
		avg += grades[i];
	}
	avg /= nStudents;
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
	double sum = 0, avg = 0;
	if(nStudents <= 1) return 0; //there's no standard deviation if there's a single value. Also avoids zero division error
	for(int i = 0; i<nRegions; i++){
		for(int j = 0; j<nCities; j++){
			for(int k = 0; k<nStudents; k++){
				avg += grades[i][j][k];
			}
		}
	}
	avg /= (nRegions * nCities * nStudents);
	for(int i = 0; i < nRegions; i++){
		for(int j = 0; j < nCities; j++){
			for(int k = 0; k < nStudents; k++){
				sum += (grades[i][j][k] - avg)*(grades[i][j][k] - avg);
			}
		}
	}
	sum /= (nRegions * nCities * nStudents - 1);
	return sqrt(sum);
}
