
#ifndef __MISC_H
#define __MISC_H

#include <db_cxx.h>
#include <map>

#ifdef CSTORE_PAPI
  #include "papi.h"
#endif

class Misc {
public:
  Misc();
  virtual ~Misc();

  //for bdb file - key comparison function
  static int compare_one_int (Db* dbp, const Dbt* a, const Dbt* b);

#ifdef CSTORE_PAPI
  //+ PAPI
  int init_PAPI();
  void setup_PAPI_event();
  void start_PAPI_event();
  void read_PAPI_event();
  void print_PAPI_event();
  long_long get_usec();
  long_long get_cycle();
  //-
#endif

private:
  unsigned int* eventList;
  int eventSet;

#ifdef CSTORE_PAPI
  std::map<unsigned int, long_long> eventInfo;
#endif

};

#endif // __MISC_H
