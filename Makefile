sourceseq = studentsseq.c utils.c statisticsseq.c
sourcepar = studentspar.c utilspar.c statisticspar.c

all: $(sourceseq) $(sourcepar)
	gcc $(sourceseq) -g -o execseq -lm -fopenmp
	gcc $(sourcepar) -g -o execpar -lm -fopenmp

execseq: $(sourceseq)
	gcc $(sourceseq) -g -o execseq -lm -fopenmp

runseq: execseq
	./execseq < input.in

execpar: $(sourcepar)
	gcc $(sourcepar) -g -o execpar -lm -fopenmp

runpar: execpar
	./execpar < input.in
