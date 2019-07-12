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

// Central Logging Class
// Problems: migfer@mit.edu

#include "Log.h"


// Master priority threshold
#define PRIORITY_THRESHOLD 0
#define LOG_TO_FILE 10
#define LOG_FILENAME "CStoreQPTest.log"

int Log::threshold;
ostream* Log::logStream;

Log::Log()
{}
Log::~Log()
{}

void Log::logInit() {
	using namespace std;
	if (LOG_TO_FILE) {
		logStream=new ofstream();
		((ofstream*) logStream)->open(LOG_FILENAME);
	}
	else {
		logStream=&cout;
	}
}

void Log::logDestroy() {
	if (LOG_TO_FILE) {
		((ofstream*) logStream)->close();
		delete logStream;
	}
}

bool Log::writeToLog(const char* src_, int priority_, string msg_) {
	writeToLog(src_, priority_, msg_.c_str());
	return true;
}
bool Log::writeToLog(const char* src_, int priority_, const char* msg_) {
	using namespace std;
	if (priority_>=PRIORITY_THRESHOLD) {
		if (allowedSrc(src_, priority_))
			cout << "LOG: " << src_ << " MSG: " << msg_ << endl;	
	}
	return true;
}

bool Log::writeToLog(const char* src_, int priority_, const char* msg_, int val_) {
	using namespace std;
	if (priority_>=PRIORITY_THRESHOLD) {
		if (allowedSrc(src_, priority_))
			cout << "LOG: " << src_ << " MSG: " << msg_ << "=" << val_ << endl;	
	}
	return true;
}

ostream* Log::getLogStream(const char* src_, int priority_) {
	if (priority_>=PRIORITY_THRESHOLD) {
		if (allowedSrc(src_, priority_))
			return logStream;
		return NULL;
	}
	else {
		return NULL;	
	}
} 

bool Log::allowedSrc(const char* src_, int priority_) {
	using namespace std;
#ifdef CSTORE_NOLOG
	return false;
#endif
	if (strcmp(src_, "Log")==0) return true;
	else if ((strcmp(src_, "AM")==0)				&& (priority_>=0)) return false;
	else if ((strcmp(src_, "DataSource")==0)		&& (priority_>=1)) return false;

	else if ((strcmp(src_, "BitWriter")==0) 		&& (priority_>=1)) return false;
	else if ((strcmp(src_, "BitReader")==0) 		&& (priority_>=1)) return false;

	else if ((strcmp(src_, "RLEEncoder")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "RLEDecoder")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "RLEWriter")==0) 		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "RLEDataSource")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "RLEExtractLoadAndAccess")==0) && (priority_>=0)) return false;    
	else if ((strcmp(src_, "RLEDecoderII")==0)		&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "RLEEncoderII")==0)		&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "PosRLEWriter")==0)		&& (priority_>=0)) return false;    
	
	else if ((strcmp(src_, "IntEncoder")==0)		&& (priority_>=1)) return false;    
	else if ((strcmp(src_, "IntDecoder")==0)		&& (priority_>=1)) return false;    
	else if ((strcmp(src_, "IntDataSource")==0)		&& (priority_>=0)) return false;    

	else if ((strcmp(src_, "DeltaEncoder")==0)&& (priority_>=0)) 		return false;    
	
	else if ((strcmp(src_, "DeltaPosDataSource")==0)&& (priority_>=2)) return false;    
	else if ((strcmp(src_, "DeltaPosEncoder")==0)   && (priority_>=0)) return false;    
	else if ((strcmp(src_, "DeltaPosWriter")==0)	&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "DeltaPosBlock")==0)		&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "DeltaPosValue")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "PagePlacer")==0)		&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "NLJoin")==0)		&& (priority_>=0)) return false;    

	else if ((strcmp(src_, "LZEncoder")==0)		&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "LZDecoder")==0)		&& (priority_>=2)) return false;    

	else if ((strcmp(src_, "NSEncoder")==0)		&& (priority_>=1)) return false;    
	else if ((strcmp(src_, "NSDecoder")==0)		&& (priority_>=1)) return false;    

	else if ((strcmp(src_, "DictWriter")==0)	&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "DictByteWriter")==0)	&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "DictDataSource")==0)	&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "BitBlock")==0)		&& (priority_>=0)) return false;    

	else if ((strcmp(src_, "Aggregator")==0)  		&& (priority_>=1)) return false;
	else if ((strcmp(src_, "AggProcessor")==0)  		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "Query1")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query2")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query3")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query4")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query5")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query6")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query7")==0)   			&& (priority_>=0)) return false;     
	else if ((strcmp(src_, "Query3WSRS")==0)   			&& (priority_>=0)) return false;   

	else if ((strcmp(src_, "Query1U")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query2U")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query3U")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query4U")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query5U")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query6U")==0)   			&& (priority_>=0)) return false;    
	else if ((strcmp(src_, "Query7U")==0)   			&& (priority_>=0)) return false;    

	else if ((strcmp(src_, "HashTable")==0)   			&& (priority_>=0)) return false; 
	
	else if ((strcmp(src_, "Plans")==0)   			&& (priority_>=0)) return false;   
	else if ((strcmp(src_, "Plan")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "SelectPlan")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "CommitPlan")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "DeletePlan")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "InsertPlan")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "Node")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "MNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "SNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "SelectNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "InsertNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "DeleteProjectionNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "CommitNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UAggAvgNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UAggCountNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UAggMaxNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UAggMinNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UAggNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UAggSumNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UCopyNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UDeleteProjectionNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UInsertColumnNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UNegationNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "UProjectNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BAggAvgNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BAggCountNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BAggMaxNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BAggMinNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BUAggNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BAggSumNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BConjunctionNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BDisjunctionNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BJoinNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BProjectNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "MConcatNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "MDeleteProjectionNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "MInsertProjectionNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "MResultNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "SProjectionUnionNode")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "JoinOutputs")==0)		&& (priority_>=0)) return false;
	
	else if ((strcmp(src_, "Parser")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "PObject")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "BExpression")==0)		&& (priority_>=0)) return false;
	else if ((strcmp(src_, "Expression")==0)		&& (priority_>=0)) return false;
	//else if ((strcmp(src_, "")==0)		&& (priority_>=0)) return false;
	//else if ((strcmp(src_, "")==0)		&& (priority_>=0)) return false;
	
	
	else return false;	
	
}

