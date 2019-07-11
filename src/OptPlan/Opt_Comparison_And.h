#ifndef _OPT_COMPARISON_AND_H_
#define _OPT_COMPARISON_AND_H_

#include <string>

#include "Opt_Comparison.h"
#include "Opt_Object.h"

using namespace std;

class Opt_Comparison_And : public Opt_Comparison {
  
 public:
  // Default Constructor
  Opt_Comparison_And();

  Opt_Comparison_And(Opt_Object* ptrLeft, Opt_Object* ptrRight);
  
  // Default Destructor
  virtual ~Opt_Comparison_And();

  // Clone
  virtual Opt_Comparison_And* clone();

  // Clone with new table names, new table alias
  virtual Opt_Comparison_And* clone(string sTableName, string sTableAlias);
};

#endif // _OPT_COMPARISON_AND_H_
