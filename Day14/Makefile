CC = gcc
OPTFLAGS = -O6
CFLAGS = -Wunused -Wall -fopenmp   $(OPTFLAGS) 
SRCS = $(wildcard *.c)
EXECS = $(patsubst %.c,%,$(SRCS))
LIBS = -lpthread -lm

all: $(EXECS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	@-/bin/rm -f *.o *~  *.out core 

veryclean: clean
	@-/bin/rm -f $(EXECS) 

cleanlog:
	@-/bin/rm -f *.log
