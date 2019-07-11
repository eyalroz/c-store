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
#ifndef _TMMERGERTEST_H_
#define _TMMERGERTEST_H_

#include "../common/Interfaces.h"
#include "../AM/CatalogInstance.h"
#include "../AM/AM.h"
#include "../Operators/ColumnExtracter.h"
#include "../Writers/RLEWriter.h"
#include "../Wrappers/Encoder/RLEEncoder.h"
#include "../AM/PageWriter.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "../Util/StopWatch.h"
#include "../Wrappers/CodingException.h"
#include "../Wrappers/Decoder/RLEDecoder.h"
#include "../Wrappers/RLEDataSource.h"
#include "../TM/TMMerger.h"
#include "../TM/WosTuple.h"
#include "../TM/WosTupleIter.h"
#include "UnitTest.h"
#include "../AM/ROSWOSSegment.h"
#include "stdlib.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../Wrappers/RLEDataSource.h"
#include "../Util/StringUtil.h"



#define BDBTMPTABLEPATH "./RuntimeData/"
#define OUTBDBNAME "TMproj.col"


class TmMergerTest : public UnitTest
{
public:
	TmMergerTest();
	virtual ~TmMergerTest();
	bool run(Globals* g, const vector<string>&);
	bool extractAndLoad(int idx,int type, string srcFile, string bdbTableFile,Globals* g);
	string extractDelimitedSubstring( char c, string str, int index );
 private:
	string m_fileName;
	int m_col;
	string outBDBName;
};

#endif //_TmMergerTest_H_
