/* Copyright (c) 2005, Regents of Massachusetts Institute of Technology, 
 * Brandeis University, Brown University, and University of Massachusetts 
 * Boston. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   - Neither the name of Massachusetts Institute of Technology, 
 *     Brandeis University, Brown University, or University of 
 *     Massachusetts Boston nor the names of its contributors may be used 
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef MAIN_H
#define MAIN_H

#include "UnitTests/QueryTest.h"
#include "UnitTests/PlanData.h"
#include "UnitTests/OptTest.h"
#include "UnitTests/OptPlanTest.h"
#include "UnitTests/OptTestOneChoice.h"
#include "UnitTests/Plans.h"
#include "UnitTests/PlanInC.h"
#include "UnitTests/PosBlockTest.h"
#include "UnitTests/LoadTPCHData.h"
#include "UnitTests/BitReaderWriter.h"
#include "UnitTests/RLEEncodeDecode.h"
#include "UnitTests/RLEEncodeDecodeII.h"
#include "UnitTests/BDBReadWrite.h"
#include "UnitTests/RLEExtractLoadAndAccess.h"
#include "UnitTests/RLEWriterTest.h"
#include <iostream>
#include "UnitTests/GenerateBDBQueryData.h"
#include "UnitTests/JoinTest.h"
#include "UnitTests/Query1.h"
#include "UnitTests/Query2.h"
#include "UnitTests/Query3.h"
#include "UnitTests/Query4.h"
#include "UnitTests/Query4Prime.h"
#include "UnitTests/Query5.h"
#include "UnitTests/Query6.h"
#include "UnitTests/Query7.h"
#include "UnitTests/Query1U.h"
#include "UnitTests/Query2U.h"
#include "UnitTests/Query3U.h"
#include "UnitTests/Query4U.h"
#include "UnitTests/Query5U.h"
#include "UnitTests/Query6U.h"
#include "UnitTests/Query7U.h"
#include "QueryAny.h"
#include "UnitTests/Query1WSRS.h"
#include "UnitTests/Query2WSRS.h"
#include "UnitTests/Query3WSRS.h"
#include "UnitTests/MergeTest.h"
#include "UnitTests/MultipleColumnsTest.h"
#include "AM/WSMV.h"
#include "UnitTests/SpillMapTest.h"
#include "UnitTests/DeltaPosWriterTest.h"
#include "UnitTests/Type2WriterTest.h"
#include "UnitTests/RLEBlockTest.h"
#include "UnitTests/IntReaderWriterTest.h"
#include "UnitTests/IntEncodeDecodeTest.h"
#include "UnitTests/TmMergerTest.h"
#include "UnitTests/IntExtractAndLoad.h"
#include "UnitTests/DeltaPosExtractLoadAndAccess.h"
#include "UnitTests/Type2ExtractLoadAndAccess.h"
#include "UnitTests/MultiLoadAndAccess.h"
#include "UnitTests/LZEncodeDecode.h"
#include "UnitTests/LZED.h"
#include "UnitTests/NSEncodeDecode.h"
#include "UnitTests/Query100.h"
#include "compressionexps/RLEDataMaker.h"
#include "compressionexps/IntDataMaker.h"
#include "compressionexps/LZDataMaker.h"
#include "compressionexps/NSDataMaker.h"
#include "compressionexps/DictDataMaker.h"
#include "compressionexps/DictCPUDataMaker.h"
#include "compressionexps/DeltaPosDataMaker.h"
#include "compressionexps/Type2DataMaker.h"
#include "compressionexps/RLE2DataMaker.h"
#include "compressionexps/Int2DataMaker.h"
#include "compressionexps/LZ2DataMaker.h"
#include "compressionexps/NS2DataMaker.h"
#include "compressionexps/Dict2DataMaker.h"
#include "compressionexps/DictCPU2DataMaker.h"
#include "compressionexps/JoinExpDataMaker.h"
#include "compressionexps/SelExpDataMaker.h"
#include "compressionexps/RLEDataRunner.h"
#include "compressionexps/IntDataRunner.h"
#include "compressionexps/LZDataRunner.h"
#include "compressionexps/NSDataRunner.h"
#include "compressionexps/DictDataRunner.h"
#include "compressionexps/DictCPUDataRunner.h"
#include "compressionexps/DictShortCutDataRunner.h"
#include "compressionexps/DeltaPosDataRunner.h"
#include "compressionexps/Type2DataRunner.h"
#include "compressionexps/RLE2DataRunner.h"
#include "compressionexps/Int2DataRunner.h"
#include "compressionexps/LZ2DataRunner.h"
#include "compressionexps/NS2DataRunner.h"
#include "compressionexps/Dict2DataRunner.h"
#include "compressionexps/DictCPU2DataRunner.h"
#include "compressionexps/JoinExpDataRunner.h"
#include "compressionexps/JoinExpRLEDataRunner.h"
#include "compressionexps/JoinExpDictCPUDataRunner.h"
#include "compressionexps/JoinExpType2DataRunner.h"
#include "compressionexps/SelExpDataRunner.h"
#include "compressionexps/SelExpRLEDataRunner.h"
#include "compressionexps/SelExpType2DataRunner.h"
#include "compressionexps/SelExpDictCPUDataRunner.h"
#include "UnitTests/LZvsINT.h"
#include "UnitTests/DictExtractLoad.h"
#include "UnitTests/Queries.h"
#include "UnitTests/UnitTest.h"
#include "UnitTests/BitProfileTest.h"
#include "UnitTests/HashMapTest.h"
#include "UnitTests/WOSTest.h"
#include "UnitTests/DummyTest.h"
#include "UnitTests/CatalogTest.h"
#include "UnitTests/BCopyOpTest.h"
#include "UnitTests/BAndOpTest.h"
#include "UnitTests/BAndQueryTest.h"
#include "UnitTests/BOrOpTest.h"
#include "UnitTests/BNotOpTest.h"
#include "UnitTests/BNotQueryTest.h"
#include "UnitTests/BDBPerformanceProfile.h"
#include "UnitTests/InsertOpTest.h"
#include "UnitTests/DeleteOpTest.h"
#include "UnitTests/DVTest.h"
#include "UnitTests/SelectDV.h"
#include "UnitTests/DJD_Data.h"
#include "UnitTests/DJD_QueryAny.h"
#include "materialexps/BAndRLERLE.h"
#include "materialexps/MinicolConcatTest.h"
#include "materialexps/PosAndLM.h"
#include "materialexps/LMDataMaker.h"
#include "materialexps/PosAndMC.h"
#include "materialexps/PosAndEM.h"
#include "materialexps/T2Test.h"
#include "materialexps/JoinLM.h"
#include "materialexps/JoinEM.h"
#include "materialexps/JoinMC.h"
#include "sparseexps/SelExpDM.h"
#include "sparseexps/SelExpDR.h"
#include "sparseexps/StrSelExpDM.h"
#include "sparseexps/StrSelExpDR.h"
#include "UnitTests/ProjMaker.h"

class main{
public:
	main();
	virtual ~main();

};


#endif // MAIN_H
