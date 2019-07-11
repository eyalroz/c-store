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


#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <cstring>
#include <fstream>

// Priority 0	Low level debug (i.e. everything)
// 			1	Debug
using namespace std;
class Log{
public:

	Log();
	virtual ~Log();
	static int threshold;
	
	// these need to be called in main (my workaround there not being static constructors 
	// for 
	static void logInit();
	static void logDestroy();
	
	static bool writeToLog(char* src_, int priority_, char* msg_);
	static bool writeToLog(char* src_, int priority_, string msg_);
	static bool writeToLog(char* src_, int priority_, char* msg_, int var_);

	// Get an output stream on which to print log messages
	// Return pointer may be NULL (in the case log messages from this src
	// and with this priority have been suppressed
	static ostream* getLogStream(char* src_, int priority);
	
protected:
	static ostream* logStream;
	static bool allowedSrc(char* src_, int priority_);
};

#endif // LOG_H

