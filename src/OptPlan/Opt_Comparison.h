/*
 *  Opt_Comparison.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a comparison
 */

  
#ifndef _OPT_COMPARISON_H_
#define _OPT_COMPARISON_H_
 
#include <string>

#include "Opt_Object.h"
#include "Opt_Column.h"

using namespace std;

class Opt_Comparison : public Opt_Object {		
 protected:
  // Member declaration
  Opt_Object*	m_ptrLeft;
  Opt_Object*	m_ptrRight;
  string m_sOp;

 public:
  // Default constructor
  Opt_Comparison();
  
  // Provide column and table names
  Opt_Comparison(Opt_Object* ptrLeft, Opt_Object* ptrRight, string sOp);
    
  // Default Destructor
  virtual ~Opt_Comparison();
  
  // Clone
  virtual Opt_Comparison* clone();

  // Clone with new table names, new table alias
  virtual Opt_Comparison* clone(string sTableName, string sTableAlias);
  
  void setLeft(Opt_Object* ptrLeft);
  Opt_Object* getLeft();
  
  void setRight(Opt_Object* ptrRight);
  Opt_Object* getRight();

  void setOp(string sOp);
  string getOp();
  
  virtual string toString();

  // Return a string presentation for this aggregate
  virtual string toStringNoTableDot();
};

#endif // _OPT_COMPARISON_H_
