CC = gcc
CFLAGS = -g -Wall -lpthread -lm

###
# Setting default make
###
default: datatrim serialtester main
all: datatrim serialtester main

###
# Support
###
datatrim:
	$(CC) -o datatrim sdk/datatrim.c

serialtester:
	$(CC) -o serialtester sdk/serialtester.c sdk/Lab4_IO.c

main:
	mpicc $(CFLAGS) -o main main.c sdk/Lab4_IO.c

###
# Clean process
###
clean:
	rm -f datatrim serialtester main
