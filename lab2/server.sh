#! /bin/bash
# For piping client processing times to a file
#
#
# Usage:
#	./server.sh
#
# Usage:
#	This shell script should be in the same directory as the server
#	implementation and have the same portnumber and number of strings
#	as the client.
#

# Params
# Port Number
port=3000
# Num Strings
arraysize=100

clear
echo "Start Server..."
script -c  "./server $port $arraysize" times.txt
