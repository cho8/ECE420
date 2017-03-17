CC := gcc
CFLAGS := -Wall -lm -fopenmp

main : parallel serial
	

sdk : datagen serialtester


clean : cleanmain cleansdk


parallel : 
	$(CC) main.c sdk/Lab3IO.c -o main $(CFLAGS)

serial :
	$(CC) main_serial.c sdk/Lab3IO.c -o main_serial $(CFLAGS)

datagen :
	$(CC) sdk/datagen.c sdk/Lab3IO.c -o datagen $(CFLAGS)

serialtester :
	$(CC) sdk/serialtester.c sdk/Lab3IO.c -o serialtester $(CFLAGS)

cleanmain :
	rm -rf *.o *.dSYM main main_serial

cleansdk :
	rm -rf datagen serialtester data_input data_output
