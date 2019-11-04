all:
	gcc studentsseq.c utils.c statisticsseq.c -g -o studentsseq -lm -fopenmp
	#gcc studentspar.c utils.c statisticsseq.c -g -o studentspar -lm -fopenmp


seq:
	gcc studentsseq.c utils.c statisticsseq.c -g -o studentsseq -lm -fopenmp

runseq: all
	./studentsseq < input.in

par:
	gcc studentspar.c utils.c statisticspar.c -g -o studentspar -lm -fopenmp

runpar:
	./studentspar < input.in
