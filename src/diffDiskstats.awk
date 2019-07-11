# awk script to diff before/after snapshot of /proc/diskstats
# assume you cat /proc/diskstats to a text file
# modify the device name below (sda) to whatever disk device you're interested in
# (Warning! --- very coarse-grained method, time spent reading/writing is not accurate. this may well be the
#  case that outstanding read/write has not yet being "counted" at the time we do the "before" snapshot.)
#
/sda / { 
  for(i=1;i<NF;i++) { 
    printf("%s ",$i);
    printf("\t");
    if (i >=4) {
      field[i] -= $i;
      field[i] = field[i] * (-1);
    }
  };
  printf("\n");
}
END { 
 print   "-------------------- Summary data for disk I/O --------------------"; 
 printf ("1. # of reads issued............................%10d\n", field[4]);
 printf ("2. # of reads merged............................%10d\n", field[5]);
 printf ("3. # of sectors read............................%10d\n", field[6]);
 printf ("4. # of milliseconds spent reading..............%10d\n", field[7]);
 printf ("5. # of writes completed........................%10d\n", field[8]);
 printf ("6. # of writes merged...........................%10d\n", field[9]);
 printf ("7. # of sectors written.........................%10d\n", field[10]);
 printf ("8. # of milliseconds spent writing..............%10d\n", field[11]);
 printf ("9. # of I/Os currently in progress..............%10d\n", field[12]);
 printf ("10 # of milliseconds spent doing I/Os...........%10d\n", field[13]);
 printf ("11.weighted # of milliseconds spent doing I/Os..%10d\n", field[14]);
 print   "-------------------------------------------------------------------"; 
 printf ("\n");
}
