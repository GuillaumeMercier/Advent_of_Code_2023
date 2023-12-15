CC = gcc
OPTFLAGS = -O6
CFLAGS = -Wunused -Wall -fopenmp   $(OPTFLAGS) 
SRCS = $(wildcard *.c) 
EXECS = $(patsubst %.c,%,$(SRCS)) day14_part2_gpu
LIBS = -lpthread -lm

all: $(EXECS) 

day14_part2_gpu: day14_part2_cuda.cu
	nvcc -o $@ $< -lcudart

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	@-/bin/rm -f *.o *~  *.out core 

veryclean: clean
	@-/bin/rm -f $(EXECS) 

cleanlog:
	@-/bin/rm -f *.log
