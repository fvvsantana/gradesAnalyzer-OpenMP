#ifndef STATISTICSSEQ_H
#define STATISTICSSEQ_H

int find_min(int* grades, int nStudents);
int find_max(int* grades, int nStudents);
double find_median(int* grades, int nStudents, int range);
double calculate_average(int* grades, int nStudents);
double calculate_stddev(int* grades, int nStudents);

#endif // STATISTICSSEQ_H
