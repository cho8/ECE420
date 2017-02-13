#! /bin/bash
# Run your client for 100 times
# 
# 
# Usage:
#	./test.sh
# Notes:
#	This shell script should be in the same directory as your
#	client implementation and your client should have portnumber 
#	and the number of strings in theArray as command line paramters;
#


# Parameters
Duplicates=100
# Port Number
port=22222

# The number of strings in theArray
arraysize=100

clear

echo "Start..."
ATTEMPT=0
while [[ $ATTEMPT -ne $Duplicates ]]; do
	let ATTEMPT+=1
	echo $ATTEMPT
	./client $port $arraysize
	sleep .5
done
