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
#include "HashMapTest.h"

HashMapTest::HashMapTest() {
  map = new HashMap(10);
}

HashMapTest::~HashMapTest() {
  delete map;
}

bool HashMapTest::run(Globals *g, const vector<string>&) {
  
  map->setHashFunction(new IdentityHashFunction(4));
	
  int a = 5, b = 1, c = 2, d = 3;
  IntValPos w(0,5);
  IntValPos x(0,1);
  IntValPos y(0,2);
  IntValPos z(0,3);
  map->put(&x, (void*)&c, static_cast<unsigned int>(sizeof(int)));
  map->put(&y, (void*)&d, static_cast<unsigned int>(sizeof(int)));
  map->put(&z, (void*)&b, static_cast<unsigned int>(sizeof(int)));
  

  if (map->get(&x) != &c) return false;
  if (map->get(&y) != &d) return false;
  if (map->get(&z) != &b) return false;
  if (map->getDataSize() != 3*sizeof(int)) return false;
  if (map->getNumKeys() != 3) return false;

  map->put(&x, (void*)&a, sizeof(int));
  if (map->get(&x) != &a) { return false; }
  if (map->getDataSize() != 3*sizeof(int)) return false;

  if (map->getNumKeys() != 3) return false;

  map->put(&w, (void*)&c, sizeof(int));
  if (map->get(&x) != &a) return false;
  if (map->get(&w) != &c) return false;
  if (map->getDataSize() != 4*sizeof(int)) return false;
  if (map->getNumKeys() != 4) return false;

  for (int i = 0; i < 4; i++) {
    ValPos* k = map->getKey()->clone();
    void* removed = map->remove(k);
    cout << "HashMap: iterated over (" << *(int*)k->value << "," << removed << ")\n";
    delete k;
    //cout << "NumKeys: " << map->getNumKeys() << "\n";
  }
  
  /*cout << "HashMap performance test" << endl;
  HashMap* map2 = new HashMap(100000);
  map2->setHashFunction(new IdentityHashFunction(100000));
  cout << "Finished instatiation" << endl;
  
  for (int i=0; i<100000; i++) {
	map2->put(i, (void*) new int, 4); 
  } 
  cout << "Deleting map" << endl;

  
  if (!map->isEmpty()) return false;
  */
  return true;
}
