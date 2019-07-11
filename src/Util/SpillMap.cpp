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
// A Map that spills to disk if it gets too big
// Problems: edmond@mit.edu


#include "SpillMap.h"

SpillMap::SpillMap(int maxSize_)
{
	sizeOfData=0;
}


SpillMap::~SpillMap()
{
}


void SpillMap::putObj(int key_, void* data_, unsigned int size_) {
	multiMap.insert(make_pair(key_,Obj(data_,size_)));
	sizeOfData+=size_;
}

bool SpillMap::empty() {
	return multiMap.empty();	
}

int SpillMap::size() {
	return multiMap.size();	
}
int SpillMap::getNumObjs(int key_) {
	return multiMap.count(key_)	;
}


int SpillMap::getObjsSize(int key_) {
   int size=0;
   multimap<int,Obj>::iterator pos;
   for (pos = multiMap.lower_bound(key_);
         pos != multiMap.upper_bound(key_); ++pos) {
            size+=((Obj) pos->second).size;
    }
    return size;
}


void* SpillMap::getObj(int key_) {
	return ((Obj) multiMap.find(key_)->second).obj;
}


int SpillMap::getObjSize(int key_) {
	return ((Obj) multiMap.find(key_)->second).size;
}


void* SpillMap::getObj(int key_, unsigned int pos_) {
	return ((Obj) getIterator(key_, pos_)->second).obj;
}

int SpillMap::getObjSize(int key_, unsigned int pos_) {
	return ((Obj) getIterator(key_, pos_)->second).size;
}

void* SpillMap::removeFirstObj() {
	void* obj=((Obj) multiMap.begin()->second).obj;
	sizeOfData-=((Obj) multiMap.begin()->second).size;
	multiMap.erase(multiMap.begin());	
	return obj;
}

void*  SpillMap::removeObj(int key_) {
	multimap<int, Obj>::iterator pos=multiMap.find(key_);
	sizeOfData-=((Obj) pos->second).size;
	void* obj=((Obj) pos->second).obj;
	multiMap.erase(pos);		
	return obj;
}

void* SpillMap::removeObj(int key_, unsigned int pos_) {
	multimap<int, Obj>::iterator pos=getIterator(key_, pos_);
	sizeOfData-=((Obj) pos->second).size;
	void* obj=((Obj) pos->second).obj;
	multiMap.erase(pos);		
	return obj;
}
void  SpillMap::clearObjs(int key_) {   
   multimap<int,Obj>::iterator pos;
   for (pos = multiMap.lower_bound(key_);
         pos != multiMap.upper_bound(key_); ++pos) {
    	sizeOfData-=((Obj) pos->second).size;
		multiMap.erase(pos);	
   }
}


multimap<int,SpillMap::Obj>::iterator SpillMap::getIterator(int key_, int pos_) {
 	int count=0;
  	multimap<int,Obj>::iterator pos;
   	for (pos = multiMap.lower_bound(key_);
    	pos != multiMap.upper_bound(key_); ++pos) {
		if (count==pos_)
			return pos;
		count++;
   }	
   throw new UnexpectedException("SpillMap: Error, this position does not exist for this key");	
}

