
main : 
	gcc -g -Wall -o main main.c sdk/lab1_IO.c -lpthread -lm 

matrixgen : 
	gcc sdk/matrixgen.c -o matrixgen

serialtester : 
	gcc sdk/serialtester.c sdk/lab1_IO.c -o serialtester


clean :
	rm -rf *.o *.dSYM main matrixgen serialtester data_input data_output
