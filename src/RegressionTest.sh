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
echo "how many queries?"; read X
echo $X | grep [^0-9] > /dev/null 2>&1
if [ "$?" -eq "0" ]
  then # If the grep found something other than 0-9, then it's not an integer.
    echo "Sorry, needed a number, try again..."
    exit 1
fi
#
LOGFILE=
numQueries=$X
i=0
if (($# == 1)) && (($1 > 0)) 
    then
	i=$1
    else
        i=1
fi
failed=0
while [ $i -le "$numQueries" ]
  do
    echo "processing Query ${i}..."
    if [[ -e ccoderoswos/q${i}.ccode ]]
      then 
	echo "compiling QueryAny with ccoderoswos/q${i}.ccode..."
        cp ccoderoswos/q${i}.ccode QueryPlan.ccode
        make > /dev/null 2>&1
	echo "running query ${i}..."
        \time ./cstoreqptest 0 qa.cnf global.cnf > /dev/null 2>&1
        cp QueryX.out ccoderoswos/q${i}test.out
	if [[ -e ccoderoswos/Query${i}.ans ]]
	    then
	    if cmp QueryX.out ccoderoswos/Query${i}.ans &> /dev/null  # Suppress output.
		then echo "Query${i} test result MATCHED !"
	        else echo "Query${i} test result NOTMATCHED !"
		failed=1
	    fi
	fi
      else
        echo "Warnning: q${i}.ccode not found..."     
    fi
    i=`expr $i + 1`
  done
  echo -n "script completed: "; cat </dev/tcp/time.nist.gov/13
  if ((failed==1))
    then
      echo "SOME TESTS FAILED"
    else
      echo "ALL TESTS SUCCEEDED"
  fi
#
