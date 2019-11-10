sourceseq = studentsseq.c utils.c statisticsseq.c
sourcepar = studentspar.c utilspar.c statisticspar.c
flags = -Wall -Wextra -Wpedantic -Werror -O2

all: $(sourceseq) $(sourcepar)
	gcc $(sourceseq) $(flags) -o execseq -lm -fopenmp
	gcc $(sourcepar) $(flags) -o execpar -lm -fopenmp

execseq: $(sourceseq)
	gcc $(sourceseq) $(flags) -o execseq -lm -fopenmp

runseq: execseq
	./execseq < input.in

execpar: $(sourcepar)
	gcc $(sourcepar) $(flags) -o execpar -lm -fopenmp

runpar: execpar
	./execpar < input.in

debug: flags += -g
debug: all
