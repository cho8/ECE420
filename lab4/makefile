CC = gcc
CFLAGS = -g -Wall -lpthread -lm

all: datatrim serialtester main


datatrim:
	$(CC) -o datatrim sdk/datatrim.c

serialtester:
	$(CC) -o serialtester sdk/serialtester.c sdk/Lab4_IO.c -lm

main:
	mpicc $(CFLAGS) -o main main.c sdk/Lab4_IO.c

reduce:
	mpicc $(CFLAGS) -o main_reduce main_reduce.c sdk/Lab4_IO.c

clean:
	rm -f datatrim serialtester main main_reduce
