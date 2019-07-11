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
/*
 *  PlanData.h
 *  cstore
 *
 *  Created by Nga Tran on 4/22/05.
 *  
 *	Generate data for plan tests
 */

#ifndef _PLANDATA_H_
#define _PLANDATA_H_

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
#include "../AM/WOSAM.h"
#include "../AM/WOSManager.h"

class PlanData : public UnitTest
{
public:
	PlanData();
	virtual ~PlanData();
	bool run(Globals* g, const vector<string>&);
	bool generate_D1_l_shipdate(Globals* g);
	bool generate_D1_l_suppkey(Globals* g);
	
	bool generate_D2_o_orderdate(Globals* g);
	bool generate_D2_l_shipdate(Globals* g);
	bool generate_D2_l_suppkey(Globals* g);
	
	/*
	bool generate_D3_o_orderkey(Globals* g);
	bool generate_D3_o_orderdate(Globals* g);
	bool generate_D3_o_custkey(Globals* g);
	*/
	
	bool generate_D4_l_extendedprice(Globals* g);
	bool generate_D4_l_returnflag(Globals* g);
	bool generate_D4_c_nationkey(Globals* g);
	
	/*
	bool generate_D5_c_custkey(Globals* g);
	bool generate_D5_c_nationkey(Globals* g);
	*/	
};

class PlanDataFullScale : public UnitTest
{
public:
	PlanDataFullScale();
	virtual ~PlanDataFullScale();
	bool run(Globals* g, const vector<string>&);
	bool generate_D1_l_shipdate(Globals* g);
	bool generate_D1_l_suppkey(Globals* g);
	
	bool generate_D1_l_orderkey(Globals* g);
	bool generate_D1_l_partkey(Globals* g);
	bool generate_D1_l_linenumber(Globals* g);
	bool generate_D1_l_quantity(Globals* g);
	bool generate_D1_l_extendedprice(Globals* g);
	bool generate_D1_l_returnflag(Globals* g);
	
	bool generate_D2_o_orderdate(Globals* g);
	bool generate_D2_l_shipdate(Globals* g);
	bool generate_D2_l_suppkey(Globals* g);
	
	bool generate_D4_l_extendedprice(Globals* g);
	bool generate_D4_l_returnflag(Globals* g);
	bool generate_D4_c_nationkey(Globals* g);
};

#endif //_PLANDATA_H_


