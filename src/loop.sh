#!/bin/bash
#
trap abort_script 2 # ctrl-c
#
abort_script()
{
  echo "ctrl-c detected! aborting..."
  exit 1
}
#
echo "loop how many times?"; read X
echo $X | grep [^0-9] > /dev/null 2>&1
if [ "$?" -eq "0" ]
  then # If the grep found something other than 0-9, then it's not an integer.
    echo "Sorry, needed a number, try again..."
    exit 1
fi
#
numQueries=$X
i=1
while [ $i -le "$numQueries" ]
  do
        echo "flushing cache..."
        cd ../data
        if [ -f lgFile ]
          then :
          else ./makeLargeFile > /dev/null 2>&1
        fi
        ./flushCache
        cd ../src
        echo "running loop ${i}..." 
	vmstat >> loop${i}time.txt
        \time ./cstoreqptestOpt >> loop${i}time.txt
        vmstat >> loop${i}time.txt
	i=`expr $i + 1`
  done
  echo -n "script completed: "; cat </dev/tcp/time.nist.gov/13
#
