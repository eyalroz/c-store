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


#ifndef _SPILLMAP_H_
#define _SPILLMAP_H_


#include <map>
#include <vector>
#include "../common/UnexpectedException.h"


using namespace std;
/* Implements a Map with key=int, object=vector
 * Every insertion must specify the number of bytes we inserted
 */

class SpillMap
{
public:
	// zero indicates unlimited size
	SpillMap(int maxSize_);
	virtual ~SpillMap();

	void putObj(int key_, void* obj_, unsigned int size_);

	bool empty();
	int size();
	int getNumObjs(int key_);
	int getObjsSize(int key_);

	void* getObj(int key_);
	int getObjSize(int key_);
	
	void* getObj(int key_, unsigned int pos_);
	int getObjSize(int key_, unsigned int pos_);
	
	void* removeFirstObj();
	
	void* removeObj(int key_);
	void* removeObj(int key_, unsigned int pos_);
	void clearObjs(int key_);
	
	
	
protected:
	struct Obj {
		Obj() {}
		Obj(void* obj_, unsigned int size_) : size(size_), obj(obj_) {}
		unsigned int size;
		void* obj;
	};
	
	multimap<int,Obj>::iterator getIterator(int key_, int pos_);
	
	int sizeOfData;
	multimap<int, Obj> multiMap;
	
};

#endif //_SPILLMAP_H_
