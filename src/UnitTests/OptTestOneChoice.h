#ifndef _OPTTESTONECHOICE_H_
#define _OPTTESTONECHOICE_H_

#include ".././OptPlan/Opt_SnowFlakes.h"
#include ".././OptPlan/Opt_QueryGraph.h"
#include ".././OptPlan/Opt_Comparison_Eq.h"
#include ".././OptPlan/Opt_Table.h"
#include ".././OptPlan/Opt_String.h"
#include ".././OptPlan/Opt_Column.h"
#include ".././OptPlan/Opt_Join.h"
#include ".././OptPlan/Opt_Comparison_Ge.h"
#include ".././OptPlan/Opt_Comparison_Lt.h"
#include ".././OptPlan/Opt_Comparison_And.h"
#include <iostream>
#include "../parser/Parser.h"
#include "../common/Util.h"

//#include<stack>

//#include "../parser/PObject.h"
//#include "../parser/Parser.h"
//#include "../plan/BaseNode.h"

//#include "../common/Interfaces.h"
//#include "../AM/AM.h"
//#include "../Operators/ColumnExtracter.h"
//#include "../Writers/RLEWriter.h"
//#include "../Wrappers/Encoder/RLEEncoderII.h"
//#include "../AM/PageWriter.h"
//#include "../common/Constants.h"
//#include "../common/Util.h"
//#include "../Wrappers/CodingException.h"
//#include "../Wrappers/Decoder/RLEDecoderII.h"
//#include "../Wrappers/RLEIIDataSource.h"
//#include "../Paths.h"
//#include "../Operators/BlockPrinter.h"
//#include "../Operators/Count.h"
//#include "../AM/PagePlacer.h"
//#include <ctime>
#include "UnitTest.h"

class OptTestOneChoice : public UnitTest
{
 public:
  OptTestOneChoice();
  ~OptTestOneChoice();

  bool run(Globals* g, const vector<string>& args);
};

#endif // _OPTTESTONECHOICE_H_
