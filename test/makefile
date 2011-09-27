
# gcc -I ../gopts/include -o test test.c ../gopts/libgopts.a

PROGRAM = test
CFLAGS = -g -Wall -I ../gopts/include
LIB = ../gopts/libgopts.a

$(PROGRAM): test.c
	$(CC) $(CFLAGS) -o $@ $< $(LIB)

clean:
	rm test

