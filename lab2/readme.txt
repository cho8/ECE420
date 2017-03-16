To compile all files, run:

make

To remove all object files and executables, run:

make clean


To run the server with mutex implementation, run :

./server <port number> <number of strings>

To run the server with read-write lock optimization, run:

./server <port number> <number of strings>


To run the client:
./client <port number> <number of strings>
