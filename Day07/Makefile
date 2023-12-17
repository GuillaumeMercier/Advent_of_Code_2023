CC = gcc
OPTFLAGS = -O0 
CFLAGS = -Wunused -Wall  -g $(OPTFLAGS) 
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
