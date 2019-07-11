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
LOGFILE=
numQueries=17
i=0
if (($# == 1)) && (($1 > 0)) 
    then
	i=$1
    else
        i=1
fi
echo "Nightly Regression Test" > QueryAny.dat
date >> QueryAny.dat
failed=0
while [ $i -le "$numQueries" ]
  do
    echo "processing Query ${i}..." >> QueryAny.dat
    if [[ -e ccoderoswos/q${i}.ccode ]]
      then 
	#echo "compiling QueryAny with ccoderoswos/q${i}.ccode..." >> QueryAny.dat
        cp ccoderoswos/q${i}.ccode QueryPlan.ccode
        make > /dev/null 2>&1
	#echo "running query ${i}..." >> QueryAny.dat
        \time ./cstoreqptest 0 qa.cnf global.cnf > /dev/null 2>&1
        cp QueryX.out ccoderoswos/q${i}test.out
	if [[ -e ccoderoswos/Query${i}.ans ]]
	    then
	    if cmp QueryX.out ccoderoswos/Query${i}.ans &> /dev/null  # Suppress output.
		then echo "Query${i} test result MATCHED !" >> QueryAny.dat
	        else echo "Query${i} test result NOTMATCHED !" >> QueryAny.dat
		failed=1
	    fi
	fi
      else
        echo "Warnning: q${i}.ccode not found..."  >> QueryAny.dat
    fi
    i=`expr $i + 1`
  done
#  echo -n "script completed: "; cat </dev/tcp/time.nist.gov/13
  if ((failed==1))
    then
      echo "SOME TESTS FAILED" >> QueryAny.dat
    else
      echo "ALL TESTS SUCCEEDED" >> QueryAny.dat
  fi
#
