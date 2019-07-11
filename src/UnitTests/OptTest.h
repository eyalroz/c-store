/* Test File for Opt_Plan/
 *
 * Email questions to hoangt@brandeis.edu
 */

#ifndef _OPTTEST_H_
#define _OPTTEST_H_

#include <iostream>
#include <string>
#include <assert.h>
#include <time.h>

#include "../common/Interfaces.h"
#include "../AM/AM.h"
#include "../Operators/ColumnExtracter.h"
#include "../Writers/RLEWriter.h"
#include "../Wrappers/Encoder/RLEEncoderII.h"
#include "../AM/PageWriter.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "../Wrappers/CodingException.h"
#include "../Wrappers/Decoder/RLEDecoderII.h"
#include "../Wrappers/RLEIIDataSource.h"
#include "../Paths.h"
#include "../Operators/BlockPrinter.h"
#include "../Operators/Count.h"
#include "../AM/PagePlacer.h"
#include <ctime>
#include "UnitTest.h"

#include "../plan/BaseNode.h"
#include ".././parser/PObject.h"
#include ".././parser/Parser.h"
#include ".././parser/Queries/QSelect.h"

#include ".././OptPlan/Opt_Object.h"
#include ".././OptPlan/Opt_QueryGraph.h"
#include ".././OptPlan/Opt_SnowFlakes.h"


int yyparse (void);

class OptTest : public UnitTest {
  
 public:
  OptTest();
  ~OptTest();

  bool run(Globals* g, const vector<string>& args);

  // Decompose a given query
  void decompose(Opt_QueryGraph* ptrQueryGraph, list<char*> factTables, char queryOutputName[256]);
  
 private:
  
};

#endif // _OPTTEST_H_
