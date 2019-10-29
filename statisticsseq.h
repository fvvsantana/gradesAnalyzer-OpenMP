#ifndef STATISTICSSEQ_H
#define STATISTICSSEQ_H

#include <stdio.h>
#include <math.h>

int find_min(int* grades, int nStudents);
int find_max(int* grades, int nStudents);
double find_median(int* grades, int nStudents, int range);
double calculate_average(int* grades, int nStudents);
double calculate_stddev(int* grades, int nStudents);

double find_min_double(double* grades, int nStudents);
double find_max_double(double* grades, int nStudents);
double find_median_double(double* grades, int nStudents, int range);
double calculate_average_double(double* grades, int nStudents);
double calculate_stddev_double(double* grades, int nStudents);

#endif // STATISTICSSEQ_H
