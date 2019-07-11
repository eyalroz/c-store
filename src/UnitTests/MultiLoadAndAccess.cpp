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
#include "MultiLoadAndAccess.h"

MultiLoadAndAccess::MultiLoadAndAccess()
{
}

MultiLoadAndAccess::~MultiLoadAndAccess()
{
}

bool MultiLoadAndAccess::run(Globals* g, const vector<string>& args) {
	string path=SHIPDATE_MINI;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	IntEncoder* encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
	IntDecoder* decoder=new IntDecoder(true);
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
	pagePlacer->placeColumn("IntExtractAndLoadDATA", false, true);
	delete pagePlacer;
	
	ROSAM* am = new ROSAM( "IntExtractAndLoadDATA" , 2);
	byte* page=(byte*) am->getNextPage();
	page=(byte*) am->getNextPage();
	page=(byte*) am->getNextPage();
	
	if (page==NULL) 
		cout << "Page was NULL" << endl;
	return true;
}

void MultiLoadAndAccess::printBlock(Block* block_) {
	if (block_!=NULL) {
		while (block_->hasNext()) {
			ValPos* pair=block_->getNext();
			assert(pair->type == ValPos::INTTYPE);
			if (pair!=NULL) cout << "(" << *(int*)pair->value << "," << pair->position << ")" << endl;
		}	
	}
}
