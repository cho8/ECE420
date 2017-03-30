#! /bin/bash
# Evaluate the assignment of Lab 3
# 
# yaochen@ualberta.ca
# Created Feb 16 2016   
# Modified Feb 16 2016
# Modified Mar 15 2017 (huiquan@ualberta.ca)
# 
# Usage:
#	./check.sh
# Notes:
#	This shell script should be in the same directory as the
#	exacutables of datatrim, serialtester and your solution.
#


## Start up
# Welcome
echo "=====Evaluation for Lab 4 code.====="
# Parameters
Sizes=(5300 13000)
Cores=(1 4)
Duplicates=4

# Create temporary directory for testing and copy all exacutable to there.
mkdir tmp

echo "Enter the exacutable file name for generating data and press [ENTER] (default is datatrim):"
read mxgenEX
if [ "$mxgenEX" = "" ]; then
	mxgenEX=datatrim
fi
if [ ! -f $mxgenEX ]; then
	echo "File $mxgenEX does not exist! Exiting............"
	exit
fi
cp $mxgenEX tmp/

echo "Enter the exacutable file name for serial testing and press [ENTER] (default is serialtester):"
read srltstEX
if [ "$srltstEX" = "" ]; then
	srltstEX=serialtester
fi
if [ ! -f $srltstEX ]; then
	echo "File $srltstEX does not exist! Exiting............"
	exit
fi
cp $srltstEX tmp/

echo "Enter the exacutable file name for matrix multiplication and press [ENTER] (default is main):"
read mainEX
if [ "$mainEX" = "" ]; then
	mainEX=main
fi
if [ ! -f $mainEX ]; then
	echo "File $mainEX does not exist! Exiting............"
	exit
fi
cp $mainEX tmp/
cp web-Stanford.txt tmp/

cd tmp/
## calculation

# for different sizes
echo "Evaluating the results..."
    # real calculation
    #chmod 755 tmp/main
    for SAMPLE in ${Sizes[@]}; do
	echo "Generating the testing data..."
    	./$mxgenEX -b $SAMPLE
        echo "SAMPLE $SAMPLE"
        for CORE in ${Cores[@]}; do
            echo "$CORE threads"
            #BestTime=10000000;
            RecordTime=10000000;
            ATTEMPT=0
            while [[ $ATTEMPT -ne $Duplicates ]]; do
                let ATTEMPT+=1
		        echo -n "Attempt Number $ATTEMPT: "
                rm -f data_output
		        mpirun -np $CORE ./$mainEX >/dev/null
                ./$srltstEX
                stat=$?
                if [[ $stat -eq "0" ]]; then
                    # correct result, collect the time
                    #temp=$(tail -n 1 data_output)
					temp=$(sed -n '2p' < data_output)
                    if [ $(echo "$temp < $RecordTime" | bc) -eq "1" ]; then
                        RecordTime=$temp
                    fi
                else
                    # wrong result, record the state
                    if [[ $stat -eq "-254" ]]; then
                        BestTime="-2"
                    fi
                    if [[ $stat -eq "1" ]] || [[ $stat -eq "2" ]]; then
                        BestTime="-3"
                    fi
                    ATTEMPT=$Duplicates
                fi
            done
            # record the result
		echo "Time of calculation: $RecordTime"
        done
    done

## post-processing
cd ..
rm -rf tmp
