#!/bin/bash
# (Note: you need to have 1. PAPI installed to run this script  2. su privilege)
# script to compile a c-store "stock" query, runs it n times. 
# P1=query number, P2=number of iteration, P3=flag to flush cache
# output is stored as qXPAPI.Y, where X=query number and Y is the iTH iteration
#
trap abort_script 2 # ctrl-c
#
abort_script()
{
  echo "ctrl-c detected! aborting..."
  exit 1
}
#
LOGFILE=
# P1=query number
queryNumber=1
if (($# >= 1)) && (($1 > 0))
    then
	queryNumber=$1
    else
        queryNumber=1
fi
# P2=number of iteration
numIteration=1
if (($# >= 2)) && (($2 > 0))
    then
	numIteration=$2
    else
        numIteration=1
fi
# P3=flag to flush cache in between runs(1=Yes, 0=No)
flushCache=0
if (($# >= 3)) && (($3 > 0))
    then
	flushCache=1
    else
        flushCache=0
fi
#
i=1
echo "test subject is Query ${queryNumber}, will iterate ${numIteration} time(s)..."
while [ $i -le "$numIteration" ]
  do
    if [[ -e q${queryNumber}.ccode ]]
      then 
	if ((${i}==1))
	    then
	        echo "compiling QueryAny with q${queryNumber}.ccode..."
	        cp q${queryNumber}.ccode QueryPlan.ccode
	        make papi > /dev/null 2>&1
	    else
	        echo ""
	fi
	if ((($flushCache)>0)) 
	    then 
	        echo "flushing cache..."
		./doFlushCache
	    else
	        echo "NO flushing cache..."
	fi
	echo "running query ${queryNumber}...(run# ${i})"
        \time ./cstoreqptestPAPI 0 qa.cnf global.cnf > q${queryNumber}PAPI.${i}
        cp QueryX.out q${queryNumber}test.out
	if [[ -e Query${queryNumber}.ans ]]
	    then
	    if cmp QueryX.out Query${queryNumber}.ans &> /dev/null  # Suppress output.
		then echo "Query${queryNumber} test result MATCHED !"
	        else echo "Query${queryNumber} test result NOTMATCHED !"
	    fi
	fi
      else
        echo "Warnning: q${queryNumber}.ccode not found..."     
    fi
    i=`expr $i + 1`
  done
  echo -n "script completed: "; cat </dev/tcp/time.nist.gov/13
#
