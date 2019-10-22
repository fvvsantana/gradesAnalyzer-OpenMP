seq:
	gcc studentsseq.c utils.c -o studentsseq -lm -fopenmp

runseq:
	./studentsseq < input.in
