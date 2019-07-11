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
numQueries=$X
i=1
while [ $i -le "$numQueries" ]
  do
    echo "processing Query ${i}..."
    if [[ -e q${i}.ccode ]]
      then 
        echo "compiling QueryAny with q${i}.ccode..."
        cp q${i}.ccode QueryPlan.ccode
        make gprof > /dev/null 2>&1
        [ -e q${i}gprof.txt ] && rm q${i}gprof.txt
        [ -e gmon.out ] && rm gmon.out
        echo "flushing cache..."
        cd ../data
        if [ -f lgFile ]
          then :
          else ./makeLargeFile > /dev/null 2>&1
        fi
        ./flushCache
        cd ../src
        echo "running query ${i}..." 
        \time ./cstoreqptestGprof > /dev/null 2>&1
        echo "profiling query ${i}..."
        gprof ./cstoreqptestGprof > q${i}gprof.txt
        echo "producing output files..." 
        cp gmon.out q${i}gmon.out
        cp QueryX.out q${i}test.out
        if [[ -e Query${i}.ans ]]
          then
            echo "comparing with Query${i}.ans file..."        
            if cmp QueryX.out Query${i}.ans &> /dev/null  # Suppress output.
              then echo "Query${i} test result MATCHED !"
              else echo "Query${i} test result NOTMATCHED !"
            fi
        fi
      else
        echo "Warnning: q${i}.ccode not found..."     
    fi
    i=`expr $i + 1`
  done
  echo -n "script completed: "; cat </dev/tcp/time.nist.gov/13
#
