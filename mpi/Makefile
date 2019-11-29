sourceseq = studentsseq.c utils.c statisticsseq.c
sourcepar = utilspar.c statisticspar.c
binseq = studentsseq
binpar = studentspar
flags = -Wall -Wextra -Wpedantic  -march=native -O2

all: $(sourceseq) $(sourcepar)
	gcc $(sourceseq) $(flags) -o $(binseq) -lm -fopenmp
	mpicc $(sourcepar) studentspar.c $(flags) -o $(binpar) -lm -fopenmp
	mpicc $(sourcepar) studentsparCalculator.c $(flags)  -o studentsparCalculator -lm -fopenmp

$(binseq): $(sourceseq)
	gcc $(sourceseq) $(flags) -o $(binseq) -lm -fopenmp

runseq: $(binseq)
	./$(binseq) < input.in

$(binpar): $(sourcepar)
	mpicc $(sourcepar) studentspar.c $(flags) -o $(binpar) -lm -fopenmp
	mpicc $(sourcepar) studentsparCalculator.c $(flags)  -o studentsparCalculator -lm -fopenmp

runpar: $(binpar)
	./$(binpar) < input.in

debug: flags += -g
debug: all
