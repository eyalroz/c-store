#ifndef _OPT_COMPARISON_OR_H_
#define _OPT_COMPARISON_OR_H_

#include <string>

#include "Opt_Object.h"
#include "Opt_Comparison.h"

using namespace std;

class Opt_Comparison_Or : public Opt_Comparison {

 public:
  // Default Constructor                                                                               
  Opt_Comparison_Or();

  Opt_Comparison_Or(Opt_Object* ptrLeft, Opt_Object* ptrRight);
  
   // Default Destructor                                                                                
  virtual ~Opt_Comparison_Or();
  
  // Clone
  virtual Opt_Comparison_Or* clone();

  // Clone with new table names, new table alias
  virtual Opt_Comparison_Or* clone(string sTableName, string sTableAlias);
};

#endif // _OPT_COMPARISON_OR_H_ 
