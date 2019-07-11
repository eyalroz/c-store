#ifndef EMTUPLEPRINTER_H_
#define EMTUPLEPRINTER_H_

#include "../common/Block.h"
#include "../common/SBlock.h"
#include "../common/PosBlock.h"
#include <cassert>
#include "../common/Interfaces.h"
#include "../Wrappers/LongBasicBlock.h"
#include "Operator.h"

class TuplePrinter
{
public:
	TuplePrinter(Operator* input);
	virtual ~TuplePrinter();
	
	void printTuples(int nattrs);
	void tossTuples();
	

protected:
	Operator* input;
};

#endif /*EMTUPLEPRINTER_H_*/
