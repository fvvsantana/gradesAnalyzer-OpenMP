sourceseq = studentsseq.c utils.c statisticsseq.c
sourcepar = studentspar.c utilspar.c statisticspar.c
binseq = studentsseq
binpar = studentspar
flags = -Wall -Wextra -Wpedantic -O2 -march=native

all: $(sourceseq) $(sourcepar)
	gcc $(sourceseq) $(flags) -o $(binseq) -lm -fopenmp
	gcc $(sourcepar) $(flags) -o $(binpar) -lm -fopenmp

$(binseq): $(sourceseq)
	gcc $(sourceseq) $(flags) -o $(binseq) -lm -fopenmp

runseq: $(binseq)
	./$(binseq) < input.in

$(binpar): $(sourcepar)
	gcc $(sourcepar) $(flags) -o $(binpar) -lm -fopenmp

runpar: $(binpar)
	./$(binpar) < input.in

debug: flags += -g
debug: all
