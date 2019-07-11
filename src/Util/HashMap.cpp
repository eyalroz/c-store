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
#include "HashMap.h"
#include <iostream>

HashMap::HashMap(unsigned int numBuckets_, unsigned int dataCapacity_) {
  init(numBuckets_, dataCapacity_);
}

HashMap::~HashMap() {
  if (!_buckets)
    delete[] _intbuckets;
  else
    delete[] _buckets;
  delete playvp;
}

void HashMap::init(unsigned int numBuckets_, unsigned int dataCapacity_) {
  _numBuckets = numBuckets_;
  _dataCapacity = dataCapacity_;
  _numKeys = 0;
  _currDataSize = 0;
  _minNonEmptyBucket=numBuckets_;
//	_buckets=new void*[numBuckets_];
//	memset(_buckets,0,numBuckets_*sizeof(void*));
  _buckets = new Obj[numBuckets_];
  _intbuckets = NULL;
  playvp = new IntValPos();
  useIntObjs = false;
  _hashFunc = NULL;
  //bucketSize=new int[numBuckets_];
  //for (unsigned int i=0; i<numBuckets_; i++) {
  //  bucketSize[i]=0;
  //}
}

bool HashMap::isEmpty() {
  return _numKeys == 0;
}

void HashMap::setHashFunction(HashFunction* hashFunc) {
  _hashFunc = hashFunc;
}

unsigned int HashMap::getDataCapacity() {
  return _dataCapacity;
}

unsigned int HashMap::getDataSize() {
  return _currDataSize;
}

unsigned int HashMap::getNumBuckets() {
  return _numBuckets;
}

unsigned int HashMap::getNumKeys() {
  return _numKeys;
}

void HashMap::put(ValPos* key_, void *value_, unsigned int size_) {
  	_currDataSize += size_;
	unsigned int hashValue;
	if (key_->getSize() >= 4)
	  hashValue = (*_hashFunc)(*(int*)key_->value);//even though it might not be an int
	else if (key_->getSize() >= 2)
	  hashValue = (*_hashFunc)((int)(*(short*)key_->value)); //whatever ... fix later if need be
	else
	  hashValue = (*_hashFunc)((int)(*(byte*)key_->value)); //whatever ... fix later if need be
  	if (_minNonEmptyBucket>hashValue) _minNonEmptyBucket=hashValue;

	Obj& obj=_buckets[hashValue];
    int size=obj._count;
  	// Let's see if key exists
  	if (size==0) {
  		_numKeys++;
  		obj._count=1;
  		obj._value=value_;
  		obj._size=size_;
		obj._key=key_->clone();
		obj._next=NULL;
  	}
  	else {

	  	bool found=false;
	  	if (*obj._key==key_) {
	  		_currDataSize-=obj._size;
			obj._value=value_;
			return;
	  	}
	  	
	  	Obj* bucketPtr=obj._next;
	  	if (bucketPtr==NULL) {
	  		obj._next=new Obj(key_->clone(), value_, size_);
      		obj._count++;
    		_numKeys++;
	  	}
	  	else {
			for (int i=1; i<size && !found ; i++) {
				if (*(bucketPtr->_key)==key_) {
					_currDataSize-=bucketPtr->_size;
					bucketPtr->_value=value_;
					return;
				}
				if (i!=size-1) bucketPtr=bucketPtr->_next;
			}
    		if (!found) {
      			bucketPtr->_next=new Obj(key_->clone(), value_, size_);
      			obj._count++;
    			_numKeys++;
	    	}
	  	}
	}
}

void HashMap::put(int key_, void *value_, unsigned int size_) {
  _currDataSize += size_;
  if (!useIntObjs) {
    useIntObjs = true;
    delete[] _buckets;
    _buckets=NULL;  
    _intbuckets = new IntObj[_minNonEmptyBucket];
  }
  unsigned int hashValue = (*_hashFunc)(key_);
  if (_minNonEmptyBucket>hashValue) _minNonEmptyBucket=hashValue;
  
  IntObj& obj=_intbuckets[hashValue];
  int size=obj._count;
  // Let's see if key exists
  if (size==0) {
    _numKeys++;
    obj._count=1;
    obj._value=value_;
    obj._size=size_;
    obj._key=key_;
    obj._next=NULL;
  }
  else {
    bool found=false;
    if (obj._key==key_) {
      _currDataSize-=obj._size;
      obj._value=value_;
      return;
    }
    
    IntObj* bucketPtr=obj._next;
    if (bucketPtr==NULL) {
      obj._next=new IntObj(key_, value_, size_);
      obj._count++;
      _numKeys++;
    }
    else {
      for (int i=1; i<size && !found ; i++) {
	if (bucketPtr->_key==key_) {
	  _currDataSize-=bucketPtr->_size;
	  bucketPtr->_value=value_;
	  return;
	}
	if (i!=size-1) bucketPtr=bucketPtr->_next;
      }
      if (!found) {
	bucketPtr->_next=new IntObj(key_, value_, size_);
	obj._count++;
	_numKeys++;
      }
    }
  }
}

void* HashMap::get(ValPos* key_) {
	unsigned int hashValue;
	if (key_->getSize() >= 4)
	  hashValue = (*_hashFunc)(*(int*)key_->value);//even though it might not be an int
	else if (key_->getSize() >= 2)
	  hashValue = (*_hashFunc)((int)(*(short*)key_->value)); //whatever ... fix later if need be
	else
	  hashValue = (*_hashFunc)((int)(*(byte*)key_->value)); //whatever ... fix later if need be
  	//int hashValue = (*_hashFunc)(key_);

	Obj& obj=_buckets[hashValue];
    int size=obj._count;
  	// most often the bucket will be of size 1, so we optimize for this case
  	// plus this is a quick check
  	if (size==0) return NULL; 
  	if (size==1) {
	  if(*key_ != obj._key) {
	    return NULL;
	  }
  	  return obj._value;
  	}
  	else {
	  if (*(obj._key) == key_) {
	    return obj._value;
	  }
    	Obj* nxtObj = obj._next;
	  	bool found=false;
		for (int i=1; i<size && !found ; i++) {
			if (*nxtObj->_key==key_) {
			  return nxtObj->_value;
			}
			if (i!=size-1) nxtObj=nxtObj->_next;
		}
    	return NULL;
  }   

}

void* HashMap::get(int key_) {
  if (!_intbuckets)
    return NULL;
  unsigned int hashValue = (*_hashFunc)(key_);
  IntObj& obj=_intbuckets[hashValue];
  int size=obj._count;
  // most often the bucket will be of size 1, so we optimize for this case
  // plus this is a quick check
  if (size==0) return NULL; 
  if (size==1) {
    if(key_ != obj._key) {
      return NULL;
    }
    return obj._value;
  }
  else {
    if ((obj._key) == key_) {
      return obj._value;
    }
    IntObj* nxtObj = obj._next;
    bool found=false;
    for (int i=1; i<size && !found ; i++) {
      if (nxtObj->_key==key_) {
	return nxtObj->_value;
      }
      if (i!=size-1) nxtObj=nxtObj->_next;
    }
    return NULL;
  } 
}

void* HashMap::remove(ValPos* key_) {
	unsigned int hashValue;
	if (key_->getSize() >= 4)
	  hashValue = (*_hashFunc)(*(int*)key_->value);//even though it might not be an int
	else if (key_->getSize() >= 2)
	  hashValue = (*_hashFunc)((int)(*(short*)key_->value)); //whatever ... fix later if need be
	else
	  hashValue = (*_hashFunc)((int)(*(byte*)key_->value)); //whatever ... fix later if need be
  	//int hashValue = (*_hashFunc)(key_);
	Obj& obj=_buckets[hashValue];
    int size=obj._count;
    
	if (size==0) return NULL;
  	if (size==1) {
		void* value = obj._value;
		obj._count=0;
		_numKeys--;
		delete obj._key;
  	  	return value;
  	}
  	else if (*_buckets[hashValue]._key==key_) {
  		void* val=obj._value;
 		Obj* nxtObj = obj._next;
 		
 		obj._value=nxtObj->_value;
		delete obj._key;
		obj._key=nxtObj->_key;
		obj._size=nxtObj->_size;
		obj._next=nxtObj->_next;
		
		obj._count--;
		_numKeys--;
 		delete nxtObj;
 		return val;
  	}
  	else {
    	Obj* currObj = _buckets[hashValue]._next;
	  	Obj* prevObj=NULL;
		for (int i=1; i<size; i++) {
			if (*currObj->_key==key_) {
				void* value = currObj->_value;
				if (prevObj==NULL) 
					obj._next=currObj->_next;
				else
					prevObj->_next=currObj->_next;
				obj._count--;
				
				delete currObj->_key;
				currObj->_key=NULL;
				delete ((Obj*) currObj);
				_numKeys--;
		  	  	return value;
			}
			prevObj=currObj;
			if (i!=size-1) currObj=currObj->_next;
		}
    	return NULL;
  }   
}

void* HashMap::remove(int key_) {
  unsigned int hashValue= (*_hashFunc)(key_);
  IntObj& obj=_intbuckets[hashValue];
  int size=obj._count;
  
  if (size==0) return NULL;
  if (size==1) {
    void* value = obj._value;
    obj._count=0;
    _numKeys--;
    return value;
  }
  else if (_intbuckets[hashValue]._key==key_) {
    void* val=obj._value;
    IntObj* nxtObj = obj._next;
    
    obj._value=nxtObj->_value;
    obj._key=nxtObj->_key;
    obj._size=nxtObj->_size;
    obj._next=nxtObj->_next;
    
    obj._count--;
    _numKeys--;
    delete nxtObj;
    return val;
  }
  else {
    IntObj* currObj = _intbuckets[hashValue]._next;
    IntObj* prevObj=NULL;
    for (int i=1; i<size; i++) {
      if (currObj->_key==key_) {
	void* value = currObj->_value;
	if (prevObj==NULL) 
	  obj._next=currObj->_next;
	else
	  prevObj->_next=currObj->_next;
	obj._count--;
	delete ((Obj*) currObj);
	_numKeys--;
	return value;
      }
      prevObj=currObj;
      if (i!=size-1) currObj=currObj->_next;
    }
    return NULL;
  }   
}

ValPos* HashMap::getKey() {
  if (!useIntObjs) {
    while (_minNonEmptyBucket<_numBuckets) {
      if (_buckets[_minNonEmptyBucket]._count==0)
	_minNonEmptyBucket++;
      else {
	return _buckets[_minNonEmptyBucket]._key;
      }
    }
    throw new UnexpectedException("Error: there are no keys");
    
    return NULL;
  }
  else{
    while (_minNonEmptyBucket<_numBuckets) {
      if (_intbuckets[_minNonEmptyBucket]._count==0)
	_minNonEmptyBucket++;
      else {
	playvp->set((byte*)&(_intbuckets[_minNonEmptyBucket]._key));
	return playvp;
      }
    }
    throw new UnexpectedException("Error: there are no keys");
    
    return NULL;
  }
}

