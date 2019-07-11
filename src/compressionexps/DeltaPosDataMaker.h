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
#ifndef _DeltaPosDataMaker_H_
#define _DeltaPosDataMaker_H_

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
#include "../Wrappers/Decoder/IntDecoder.h"
#include "../Wrappers/Decoder/RLEDecoderII.h"
#include "../Wrappers/Decoder/DeltaPosDecoder.h"
#include "../Wrappers/RLEDataSource.h"
#include "../Wrappers/RLEIIDataSource.h"
#include "../Wrappers/DeltaPosDataSource.h"
#include "../Wrappers/Encoder/DeltaPosEncoder.h"
#include "../Wrappers/Encoder/IntEncoder.h"
#include "../Wrappers/Encoder/RLEEncoderII.h"
#include "../Wrappers/IntDataSource.h"
#include "../Paths.h"
#include "../Operators/BlockPrinter.h"
#include "../Operators/NLJoin.h"
#include "../Operators/Count.h"
#include "../AM/PagePlacer.h"
#include <ctime>
#include "../UnitTests/UnitTest.h"

class DeltaPosDataMaker : public UnitTest
{
public:
	DeltaPosDataMaker();
	virtual ~DeltaPosDataMaker();
	bool run(Globals* g, const vector<string>& args);
	const char* _file;

};

#endif //_DeltaPosDataMaker_H_
