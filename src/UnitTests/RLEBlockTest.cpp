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
#include "RLEBlockTest.h"

RLEBlockTest::RLEBlockTest()
{
}

RLEBlockTest::~RLEBlockTest()
{
}
/* Iterator access to block
	bool hasNext();
	Pair* getNext();

	// Zero indexed, gets the pair at this pos_ and points that 
	// Cursor to the following pair
	Pair* getPairAtLoc(int loc_);

	// return size of block		
	int getSize();
	
	bool isOneValue();
	bool isValueSorted();
		
	bool isPosContiguous();
	bool isPosSorted();
	
	RLETriple* getTriple();
	void setTriple(RLETriple* trip_);
*/

/*args isn't used here actually*/
bool RLEBlockTest::run(Globals* g, const vector<string>& args) {
	bool success=true;
	RLEBlock* block=new RLEBlock(new RLETriple(new IntValPos(40, 15), 40, 10), true);
	
	success&=test("Size", true, block->getSize(), 10);
	bool blockSuccess=true;
	for (unsigned int i=0; i<10; i++) {
		ValPos* pair=block->getNext();
		assert(pair->type == ValPos::INTTYPE);
		blockSuccess&=(*(int*)pair->value==15);
		blockSuccess&=(pair->position==40+i);
	}
	success&=test("All pairs in Block Match:", blockSuccess, 0, 0);
	ValPos* pair=block->getPairAtLoc(block->getSize()-1);
	assert(pair->type == ValPos::INTTYPE);
	success&=test("Value of last", true, *(int*)pair->value, 15);
	success&=test("Position of last", true, pair->position, 49);

	return success;
}

bool RLEBlockTest::test(const char* msg_, int retBool_, int val_, int exp_) {
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

