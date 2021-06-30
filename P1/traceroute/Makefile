CC = gcc
CFLAGS = -Wall -Wextra -std=gnu17

OBJ =\
	./traceroute.c \
	./send.c \
	./receive.c \

all: main

clean:
	rm -f *.o 

distclean:
	rm -f *.o traceroute

main: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -c
	$(CC) *.o $(CFLAGS) -o traceroute
