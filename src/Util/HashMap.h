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
/**
 * A HashMap is represented as an array of STL maps. The size of the array
 * corresponds to the number of hash buckets.  The user specifies a hash function
 * object to determine the hash value for each key. Each bucket stores a map 
 * from keys to Objs (which store some value and the size of the value); every
 * key in a bucket has the bucket's hash value (i.e. the array index).
 *
 * HashMaps can be augmented to support spilling to disk, but currently
 * this functionality is not supported.
 *
 * Author: Edmond Lau <edmond@mit.edu>
 * Otimized: Miguel Ferreira
 */

#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include <iterator>
#include <cassert>
#include <map>
#include <vector>

#include "HashFunction.h"
#include "../common/IntValPos.h"
#include "../common/UnexpectedException.h"

using namespace std;

class HashMap {
 public:
  HashMap(unsigned int numBuckets_, unsigned int dataCapacity_ = 100000);
  ~HashMap();
  void init(unsigned int numBuckets_, unsigned int dataCapacity_ = 100000);

  // set max memory capacity
  //void setDataCapacity(unsigned int dataCapacity_);

  bool isEmpty();

  // memory capacity in bytes
  unsigned int getDataCapacity();
  // memory footprint in bytes
  unsigned int getDataSize();

  // number of hash buckets
  unsigned int getNumBuckets();
  // number of unique keys stored
  unsigned int getNumKeys();

  void put(ValPos* key_, void *value_, unsigned int size_);
  void put(int key_, void *value_, unsigned int size_);
  void* get(ValPos* key_);
  void* get(int key_);
  void* remove(ValPos* key_);
  void* remove(int key_);

  void setHashFunction(HashFunction *hashFunc);

  ValPos* getKey();

 private:

  struct Obj {
    Obj() {
	    _count=0;
    }
    Obj(ValPos* key_, void *value_, unsigned int size_) : _key(key_), _value(value_), _size(size_) {
    	_next=NULL;
    }
	ValPos* _key;
    void *_value;
    unsigned int _size;
    Obj* _next;
    unsigned int _count;
  };

  struct IntObj {
    IntObj() {
	    _count=0;
    }
    IntObj(int key_, void *value_, unsigned int size_) : _key(key_), _value(value_), _size(size_) {
    	_next=NULL;
    }
	int _key;
    void *_value;
    unsigned int _size;
    IntObj* _next;
    unsigned int _count;
  };

  //void** _buckets;
  Obj* _buckets;
  IntObj* _intbuckets;
  //int* bucketSize;

  unsigned int _minNonEmptyBucket;
  
  unsigned int _numBuckets;
  unsigned int _numKeys;
  unsigned int _dataCapacity;
  unsigned int _currDataSize;

  HashFunction* _hashFunc;
  bool useIntObjs;
  ValPos* playvp;

};

#endif
