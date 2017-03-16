CC := gcc
CFLAGS := -Wall -lm -fopenmp

all : main serial


sdk : datagen serialtester


main:
		$(CC) main.c sdk/Lab3IO.c -o main $(CFLAGS)

serial:
		$(CC) main_serial.c sdk/Lab3IO.c -o main_serial $(CFLAGS)

datagen :
		$(CC) sdk/datagen.c sdk/Lab3IO.c -o datagen $(CFLAGS)

serialtester :
		$(CC) sdk/serialtester sdk/Lab3IO.c -o serialtester $(CFLAGS)

clean :
		rm -rf *.o *.dSYM client main
