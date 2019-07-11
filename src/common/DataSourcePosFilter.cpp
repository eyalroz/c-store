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
#include "DataSource.h"

byte* DataSource::getPageOnPos(AM* am_, Decoder* decoder_, bool posIndexPrimary_) {
	// Idea: simple, for each call to get a page we find the page containing this position
	unsigned int position;
	unsigned int filterEnd;
	
	if (isROS) {
	  if (!getPosFilterStartEnd(position, filterEnd, getNextNextTime))	
	    return NULL;	
	} else {
	  if (!getPosFilterStartEnd(position, filterEnd, true))	
	    return NULL;
	}
	
	byte* page;
	// now to get the page
	// if on same position filter block, then just walk down index
	if (!getNextNextTime || posSkippedTo==position) {
		if (posIndexPrimary_)  {
			Log::writeToLog("DataSource", 0, "Getting page on primary index");
			page=(byte*) getNextPagePosition(am_);	
		}
		else {
			Log::writeToLog("DataSource", 0, "Getting page on secondary index");
			page=(byte*) getNextPagePosition(am_);
		}
	}
	else {
	  if (position != filterEnd) // if the size of the pos filter is just 1, then we're probably not going to want to just get the next page next time - we'll want to skip again.
	    posSkippedTo=position;
		am_->initCursors();
		if (posIndexPrimary_) {
			Log::writeToLog("DataSource", 0, "Getting page on primary index at position", position);
			page=(byte*) skipToPagePosition(position, am_);
		}
		else {
			Log::writeToLog("DataSource", 0, "Getting page on secondary index at position", position);
			page=(byte*) skipToPagePosition(position, am_);
		}
	}
	return page;	
}

// Gets the range currently encoded in the position filter
bool DataSource::getPosFilterStartEnd(unsigned int& posFilterStart_, unsigned int& posFilterEnd_, bool getNextFilterRange_) {
	// Okay, do we have a range, are we being asked for next range, otherwise just return old range
	if (getNextFilterRange_ || (posFilterEnd==0)) {
		// check to see if we have a position block
	  if ((!havePosBlock) || (!currPosBlock->hasNext())) {
			getNextPosFilterBlock();
			havePosBlock=true;
			if (currPosBlock==NULL) {
			  havePosBlock=false;
			  return false;
			}
		}

		// now three cases for position blocks	
		// 1. Position contiguous
		// 2. Not Position contiguous
		if (posFilterCurrBlockPosContig /*|| (!currPosBlock->isSparse())*/) {
			// All we need is start and end
			posFilterStart=currPosBlock->getStartPos();
			posFilterEnd=currPosBlock->getStartPos()+currPosBlock->getSize()-1;		
			havePosBlock=false;
		}
		else {
			posFilterStart=currPosBlock->getStartPos();
			posFilterEnd=((PosType2Block*)currPosBlock)->getEndPos();

			/*posFilterEnd=posFilterStart;
			// now check to see if we do have a little run
			while (currPosBlock->hasNext() && currPosBlock->peekNext()==(posFilterEnd+1)) {
				posFilterEnd++;
				currPosBlock->getNext();
				}*/
			}
	}
	posFilterStart_=posFilterStart;
	posFilterEnd_=posFilterEnd;
	return (currPosBlock!=NULL);
}

// Finds the right block on the page
bool DataSource::skipToRightPosOnPage(Decoder* decoder_) {
	assert(decoder_!=NULL);
	unsigned int start=decoder_->getStartPos();	
	
	unsigned int filterStart;
	unsigned int filterEnd;
	if (!getPosFilterStartEnd(filterStart, filterEnd,false))
		return false;
	
	if (start>=filterStart) 
		return true;
	else  {
	  // assumes that positions are contiguous
		Log::writeToLog("Datasource", 0, "Skipping to loc", filterStart-start-1);
		if (decoder_->skipToPos(filterStart-start))
			return true;
		else
			return false;
	}
}

// Gets the next position block from the filter, used internally by
// getPosFilterStartEnd()
void DataSource::getNextPosFilterBlock() {	
	if (posFilter==NULL)
		currPosBlock=NULL;
	else {
		currPosBlock=posFilter->getNextPosBlock(posColIndex);
		if (currPosBlock==NULL) {
			Log::writeToLog("DataSource", 0, "GetNextPosFilterBlock() returned NULL");
			return; 
		}
		int psize = currPosBlock->getSize();
		if ((psize>1) && (psize==currPosBlock->getNumOcurrences()))
		  RLEAlways=true;
		posFilterPosSorted=currPosBlock->isPosSorted();
		posFilterCurrBlockPosSorted=currPosBlock->isBlockPosSorted();
		posFilterCurrBlockPosContig=currPosBlock->isPosContiguous();
		//cerr << "Got next posFilterBlock; sz = " << currPosBlock->getSize() << endl;
	}
}
