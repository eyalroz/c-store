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
#include "RLEVecDataSource.h"

// Construct an rle data source that :
//		-access data through a vector
//		-writes blocks using writer_
RLEVecDataSource::RLEVecDataSource(vector<RLETriple>* tripVec_){
        tripVecPtr = tripVec_;
	pageBuffer=NULL;
	valFilterChanged=false;
	posFilterChanged=false;
	predChanged=false;
	
	valueFilter=NULL;
	posFilter=NULL;
	pred=NULL;
	
	decoder=new RLEDecoder(true);
	outBlock=new RLEBlock(true);
	idx = 0;
}
	
// Copy constructor
RLEVecDataSource::RLEVecDataSource(const RLEVecDataSource& datasource_) {
  tripVecPtr = datasource_.tripVecPtr;
	pageBuffer=NULL;
	valFilterChanged=false;
	posFilterChanged=false;
	predChanged=false;
	valueFilter=NULL;
	posFilter=NULL;
	pred=NULL;
	
	decoder=NULL;
	
	idx = 0;
}
	
// Destructor
RLEVecDataSource::~RLEVecDataSource()  {

}
	

bool RLEVecDataSource::checkPage() {
  //not implmented
  return false;
}
	
// Gets the next value block from the am 
// (a block with a set of contiguous values but not necessarily contiguous positions)
Block* RLEVecDataSource::getNextValBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	
	RLETriple* triplePtr=getNextTriple();
	
	if (triplePtr==NULL) return NULL;
	else {
	  //Be careful here!!!!
	  //RLEBlock will release memory by deleting the RLEPtr that was passed in last time
	  //Can't simply pass triplePtr since the memory it points to will get deleted
	  //Notice that datasource here is actually an vector passed from outside
	  //we don't even need to delete it since it should be handled outside.

		outBlock->setTriple(new RLETriple(triplePtr->value,triplePtr->startPos,triplePtr->reps));
		return outBlock;	
	} 
		
}


RLETriple* RLEVecDataSource::getNextTriple() {
  if((unsigned int) idx < tripVecPtr->size()){
    idx++;
    return (RLETriple*)(&(*tripVecPtr)[idx-1]);

  }else{
    return NULL;
  }
  
}


RLETriple* RLEVecDataSource::getTriple(int skipVal_) {
	return NULL;
}

	
// Skips to the first block with this value
// Returns false if the value is outside the range of the column
Block* RLEVecDataSource::skipToValBlock(int colIndex_, int val_) {
	if (colIndex_!=0) return NULL;
	return NULL;
}
	
// Gets the next value block from the am 
// (a block with a set of contiguous positions but not necessarily contiguous values)
PosBlock* RLEVecDataSource::getNextPosBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	return NULL;
}

	
// Skips to the first block with this position
// Returns false if the value is outside the range of the column
// Returns false if the value does not exist in the column
// Skips to the previous block if position cannot be found for this value
PosBlock* RLEVecDataSource::skipToPosBlock(int colIndex_, int pos_) {
	if (colIndex_!=0) return NULL;
	return NULL;
}

	
// Changes the RHS binding for this datasource
void RLEVecDataSource::changeRHSBinding(int rhs_) {
	predChanged=true;
	pred->setRHS(rhs_);
}	
// sets a new predicate for this datasource
void RLEVecDataSource::setPredicate(Predicate* pred_) {
	predChanged=true;
	pred=pred_;
}
	
// Sets a filter for values
void RLEVecDataSource::setValueBitstring(DataSource* valueFilter_) {
	
}
	
// Sets a filter for postions
void RLEVecDataSource::setPositionBitstring(DataSource* posFilter_) {
	
}
void RLEVecDataSource::printColumn() {

}
