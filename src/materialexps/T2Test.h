#ifndef T2TEST_H_
#define T2TEST_H_

#include "../common/Interfaces.h"
#include "../AM/AM.h"
#include "../Operators/ColumnExtracter.h"
#include "../Writers/RLEWriter.h"
#include "../Wrappers/Encoder/RLEEncoder.h"
#include "../AM/PageWriter.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "../Wrappers/CodingException.h"
#include "../Wrappers/Decoder/RLEDecoder.h"
#include "../Wrappers/Decoder/UncompressedDecoder.h"
#include "../Wrappers/Decoder/IntDecoder.h"
#include "../Wrappers/Decoder/RLEDecoderII.h"
#include "../Wrappers/RLEDataSource.h"
#include "../Wrappers/RLEIIDataSource.h"
#include "../Wrappers/DeltaPosDataSource.h"
#include "../Wrappers/Encoder/DeltaPosEncoder.h"
#include "../Wrappers/Encoder/IntEncoder.h"
#include "../Wrappers/Encoder/RLEEncoderII.h"
#include "../Wrappers/IntDataSource.h"
#include "../Wrappers/Type2DataSource.h"
#include "../Paths.h"
#include "../Operators/BlockPrinter.h"
#include "../Operators/BAndOperator.h"
#include "../Operators/PosAndOperator.h"
#include "../Operators/Count.h"
#include "../Operators/Sum.h"
#include "../Operators/BCopyOperator.h"
#include "../AM/NOBDBAM.h"
#include <ctime>
#include "../UnitTests/UnitTest.h"
#include "../Operators/BlockWithPosToMinicolConverter.h"
#include "../Operators/Type2BlockToMinicolConverter.h"
#include "../Operators/MinicolShimOperator.h"
#include "../Wrappers/Type2DecompDataSource.h"
#include "../Operators/PosOrOperator.h"

class T2Test : public UnitTest 
{
public:
	T2Test();
	virtual ~T2Test();
	bool run(Globals* g, const vector<string>& args);
};

#endif /* T2TEST_H */
