all:
	gcc studentsseq.c utils.c statisticsseq.c -g -o execseq -lm -fopenmp
	gcc studentspar.c utilspar.c statisticspar.c -g -o execpar -lm -fopenmp

execseq:
	gcc studentsseq.c utils.c statisticsseq.c -g -o execseq -lm -fopenmp

runseq: execseq
	./execseq < input.in

execpar:
	gcc studentspar.c utilspar.c statisticspar.c -g -o execpar -lm -fopenmp

runpar: execpar
	./execpar < input.in
