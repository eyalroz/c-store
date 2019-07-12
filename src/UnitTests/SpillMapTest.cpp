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
#include "SpillMapTest.h"

SpillMapTest::SpillMapTest()
{
}

SpillMapTest::~SpillMapTest()
{
}

bool SpillMapTest::run(Globals* g, const vector<string>& args) {	
	bool success=true;
	
	byte* bfr0=new byte[PAGE_SIZE];
	byte* bfr1=new byte[80];
	byte* bfr2=new byte[PAGE_SIZE];
	byte* bfr3a=new byte[PAGE_SIZE];
	byte* bfr3b=new byte[5];
	bfr0[0]=100;
	bfr1[0]=101;
	bfr2[0]=102;
	bfr3a[0]=103;	
	bfr3b[0]=104;	
	
	SpillMap spillMap(0);
	spillMap.putObj(0, bfr0, PAGE_SIZE);
	spillMap.putObj(1, bfr1, 80);
	spillMap.putObj(2, bfr2, PAGE_SIZE);
	spillMap.putObj(3, bfr3a, PAGE_SIZE);
	spillMap.putObj(3, bfr3b, 5);
	
	success&=test("NumObjects in key=0",true, spillMap.getNumObjs(0), 1);
	success&=test("NumObjects in key=1",true, spillMap.getNumObjs(1), 1);
	success&=test("NumObjects in key=2",true, spillMap.getNumObjs(2), 1);
	success&=test("NumObjects in key=3",true, spillMap.getNumObjs(3), 2);
	success&=test("NumObjects in key=10",true, spillMap.getNumObjs(10), 0);
	
	success&=test("SizeOfObjs in key=0",true, spillMap.getObjsSize(0), PAGE_SIZE);
	success&=test("SizeOfObjs in key=1",true, spillMap.getObjsSize(1), 80);
	success&=test("SizeOfObjs in key=2",true, spillMap.getObjsSize(2), PAGE_SIZE);
	success&=test("SizeOfObjs in key=3",true, spillMap.getObjsSize(3), PAGE_SIZE+5);	
	
	success&=test("SizeOfObj in key=0",true, spillMap.getObjSize(0), PAGE_SIZE);
	success&=test("SizeOfObj in key=1",true, spillMap.getObjSize(1), 80);
	success&=test("SizeOfObj in key=2",true, spillMap.getObjSize(2), PAGE_SIZE);
	success&=test("SizeOfObj in key=3",true, spillMap.getObjSize(3), PAGE_SIZE);	
	success&=test("SizeOfObj in key=3",true, spillMap.getObjSize(3,1), 5);	
		
	success&=test("getCurrObj in key=0",true, ((byte*) spillMap.getObj(0))[0], 100);	
	success&=test("getCurrObj in key=1",true, ((byte*) spillMap.getObj(1))[0], 101);	
	success&=test("getCurrObj in key=2",true, ((byte*) spillMap.getObj(2))[0], 102);			
	success&=test("getCurrObj in key=3",true, ((byte*) spillMap.getObj(3))[0], 103);	
	spillMap.removeObj(3);
	success&=test("getCurrObj in key=3",true, ((byte*) spillMap.getObj(3))[0], 104);	
	
	return success;
}

bool SpillMapTest::test(const char* msg_, bool retBool_, int val_, int exp_) {
	using namespace std;
	if (retBool_) {
		cout << msg_ << " X: " << val_ << "\tE[X]: " << exp_ << " \t";
		if (val_==exp_) {
			cout << "SUCCESS" << endl;
			return true;
		}
		else {
			cout << "FAILED" << endl;
			return false;
		}
		
	}
	else {
		cout << "FAILED (function return failed)" <<  endl;
		return false;
	}	
}
