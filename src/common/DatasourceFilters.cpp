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

byte* DataSource::getRightPage(AM* am_, Decoder* decoder_, bool valSorted_, bool posIndexPrimary_) {
	Log::writeToLog("DataSource", 0, "Called GetRightPage()");
	// if anything has changed we need to reset the indices
	if (valFilterChanged || posFilterChanged || predChanged) {
		Log::writeToLog("Datasource", 0, "Filter or pred changed, initing cursors");
		am_->initCursors();
		valFilterChanged=false;
		posFilterChanged=false;
		predChanged=false;
		predLookup=true;
		posSkippedTo=0;
	}

	// We have 3 possible filters 
	// 1. Predicate
	// 2. Value
	// 3. Position
	if ((valFilter==NULL) && (posFilter==NULL) && (pred==NULL)) {

		if (predChanged) predChanged=false;
		return getPage(am_, decoder_);
	}
	else if ((valFilter==NULL) && (posFilter==NULL) && (pred!=NULL)) {
		if (predLookup) {
			predLookup=false;
			Log::writeToLog("DataSource", 0, "GetRightPage() calling getPageOnPred()");
			byte* page=getPageOnPred(am_, decoder_, valSorted_);
			if (page!=NULL) {
				decoder_->setBuffer(page);
			}
			return page;
		}
		else {
			Log::writeToLog("DataSource", 0, "GetRightPage() calling getPage()");
			return (byte*) getNextPageValue(am_);
		}
	}
	else if ((valFilter==NULL) && (posFilter!=NULL) && (pred==NULL)) {
		Log::writeToLog("DataSource", 0, "GetRightPage() calling getPageOnPos()");		
		return getPageOnPos(am_, decoder_, posIndexPrimary_);
	}
	else if ((valFilter==NULL) && (posFilter!=NULL) && (pred!=NULL)) {
		Log::writeToLog("DataSource", 0, "GetRightPage() calling getPageOnPosAndPred()");		
		//assuming that if posIndex is primary index then there is no value index so just get page on pos since we will have to iterate through all values anyway to apply predicate
		if (posIndexPrimary_)
		  return getPageOnPos(am_, decoder_, posIndexPrimary_);
		else
		  return getPageOnPosAndPred(am_, decoder_, posIndexPrimary_);
	}
	else if ((valFilter!=NULL) && (posFilter==NULL) && (pred==NULL)) {
		Log::writeToLog("DataSource", 0, "GetRightPage() calling getPageOnVal()");		
		return getPageOnVal(am_, decoder_, valSorted_);
	}
	else if ((valFilter!=NULL) && (posFilter==NULL) && (pred!=NULL)) {
		Log::writeToLog("DataSource", 0, "GetRightPage() calling getPageOnVal()");				
		return getPageOnVal(am_, decoder_, valSorted_);
	}
	else if ((valFilter!=NULL) && (posFilter!=NULL) && (pred==NULL)) {
		Log::writeToLog("DataSource", 0, "GetRightPage() calling getPageOnValAndPos()");		
		return getPageOnValAndPos(am_, decoder_, valSorted_);
	}
	else if ((valFilter!=NULL) && (posFilter!=NULL) && (pred!=NULL)) {
		Log::writeToLog("DataSource", 0, "GetRightPage() calling getPageOnValAndPos()");		
		return getPageOnValAndPos(am_, decoder_, valSorted_);
	}	
	throw new UnimplementedException("DataSource: to be implemented");	
}

byte* DataSource::getPage(AM* am_, Decoder* decoder_) {
	return (byte*) getNextPageValue(am_);
}
byte* DataSource::getPageOnPred(AM* am_, Decoder* decoder_, bool valSorted_) {
	if (valSorted_) {
		Log::writeToLog("DataSource", 0, "getPageOnPred(), valSorted");		
		bool lookupOnValue=false;
		//for now assume value is an integer ... fix later ...
		int value;
		if (pred!=NULL) {
			predChanged=false;
			switch	(pred->getPredType()) {
				case Predicate::OP_GREATER_THAN: 	
				case Predicate::OP_GREATER_THAN_OR_EQUAL: 	
				case Predicate::OP_EQUAL:
					{
						lookupOnValue=true;
						//for now assume value is an integer ... fix later ...
						ValPos* vp = pred->getRHS();
						assert(vp->type == ValPos::INTTYPE);
						value=*(int*)vp->value;
					}
					break;
				case Predicate::OP_LESS_THAN:
				case Predicate::OP_LESS_THAN_OR_EQUAL:
					break;
				default:
					break;
			}
		}
		if (lookupOnValue) {
			Log::writeToLog("DataSource", 1, "Getting a page on value", value);
			byte* page=((byte*) skipToPageValue((char*)&value, am_));
			return page;
		}
		else {
			
			return (byte*) getNextPageValue(am_);
		}
	}
	else {
		 return (byte*) getNextPageValue(am_);
	}
}

byte* DataSource::getPageOnPosAndPred(AM* am_, Decoder* decoder_, bool posIndexPrimary_) {
	throw new UnimplementedException("DataSource: to be implemented");	
}
byte* DataSource::getPageOnVal(AM* am_, Decoder* decoder_, bool valSorted_) {
	if (pred!=NULL) {
		
	}
	else {
		
	}
	throw new UnimplementedException("DataSource: to be implemented");
}

byte* DataSource::getPageOnValAndPos(AM* am_, Decoder* decoder_, bool valSorted_) {
	throw new UnimplementedException("DataSource: to be implemented");	
}
