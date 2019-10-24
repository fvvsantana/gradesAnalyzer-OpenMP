all:
	gcc studentsseq.c utils.c -g -o studentsseq -lm -fopenmp
	#gcc studentspar.c utils.c -o studentspar -lm -fopenmp


seq:
	gcc studentsseq.c utils.c -o studentsseq -lm -fopenmp

runseq:
	./studentsseq < input.in

par:
	#gcc studentspar.c utils.c -o studentspar -lm -fopenmp

runpar:
	#./studentspar < input.in
