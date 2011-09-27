
.SUFFIXES: .c

CC      = gcc
PROGRAM = libgopts.a
CFLAGS  = -g -Wall -I include
FILES   = strarray.o gopts.o parser.o
OBJS    = $(FILES:%=lib/%)
SRCS    = $(OBJS:%.o=src/%.c)

${PROGRAM}:$(OBJS)
	ar rv $@ $?
	ranlib $@

lib/%.o:src/%.c
	$(CC) $(CFLAGS) -o lib/$*.o -c $<

clean:
	rm $(OBJS)
	rm $(PROGRAM)



