if test ! -f $3; then echo "{print \$$2}" > dna.awk; cat $1 | awk -f dna.awk > $3; rm dna.awk; fi;
