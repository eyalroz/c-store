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
#include "NLJoin.h"
#include "../common/BlockWithPos.h"

NLJoin::NLJoin(Operator *left_, int predColIndex1_, Operator *right_, int predColIndex2_) : l_src(left_), l_predcol(predColIndex1_), r_src(right_), r_predcol(predColIndex2_) {
  
  init();
}

void NLJoin::init() {
  // set both blocks dirty
  m_isLBlockConsumed = true;
  m_isRBlockConsumed = true;
  blocks = NULL;
  // initialize input blocks
  //m_aggBlockIn = m_src->getNextValBlock(m_aggColIndex);
  //if (m_aggType == GROUP_BY_SELF) {
  //  m_groupBlockIn = m_aggBlockIn;
  //} else {
  //  m_groupBlockIn = m_group->getNextValBlock(m_groupColIndex);
  //}

  m_currOutputPosition = 1;
  //aggData = NULL;

  lBlockOut = NULL;
  rBlockOut = NULL;
  lBlockIn = NULL;
  rBlockIn = NULL;
  firstCall = true;
  dontcheck=false;
  curblocki = 0;
  indicies = new int*[200];
  for (int i = 0; i < 200; i++)
    indicies[i] = new int[2];
  for (int i = 0; i < 200; i++)
    for (int j = 0; j < 2; j++)
      indicies[i][j] = -1;
      
  currLHSTuple = NULL;
  curr_rhs_block_idx = 0;
  join_done = false;
  sblocks = NULL;
  doEM=true;
}

void NLJoin::setDontCheck() {
  dontcheck=true;
}

void NLJoin::initOutputBlocks() {
  assert(lBlockIn!=NULL);
  assert(rBlockIn!=NULL);
  assert(lBlockOut==NULL);
  assert(rBlockOut==NULL);

  if (lBlockIn->isPosContiguous()) {
    if (lBlockIn->getSize() == 1) {
      if (lBlockIn->isOneValue()) {
	if (rBlockIn->isOneValue()) {
	  if (rBlockIn->getSize() > 1) {
	  }
	}
      }
    }
    else {
      if (lBlockIn->isOneValue()) {
	if (rBlockIn->isOneValue()) {
	  if (rBlockIn->getSize() > 1) {
	  }
	}
	else{
	  lBlockOut = new PosRLEBlock();
	  rBlockOut = new PosBasicBlock(true);
	}
      }
    }
  }
  else {
    if (!rBlockIn->isOneValue()) {
      lBlockOut = new PosType2Block();
      rBlockOut = new PosBasicBlock(true);
    }
  }

  if (!lBlockOut) {
    lBlockOut = new PosBasicBlock(true);
    rBlockOut = new PosBasicBlock(true);
  }
}

NLJoin::~NLJoin() {
  /*if (aggData != NULL) {
    delete aggData;
  }*/
  if (lBlockOut != NULL) {
    delete lBlockOut;
  }
  if (rBlockOut != NULL) {
    delete rBlockOut;
    }
}

// assume that operator above will read blocks in parallel
// getNextValBlock generates output in parallel
Block* NLJoin::getNextValBlock(int colIndex_) {
  throw new UnexpectedException("NLJoin: Join only produces position blocks");
}

SBlock* NLJoin::getNextSValBlock(int colIndex_) {
  //if (doEM)
    return getNextSValBlockEM(colIndex_);
    //else
    //return getNextSValBlockMC(colIndex_);
}

Block* NLJoin::getNextValBlockMC(int colIndex_) {
  if (colIndex_==0) {
    generateBlocksMC();
    return outBlockIter;
  }
  else
    return outRLEBlock;
}

/*Block* NLJoin::getNextValBlockMC(int colIndex_) {
  assert(colIndex_==1);
  return outRLEBlock;
  }*/


/*This whole method is pretty HACKY for now. Will fix later.*/
void NLJoin::generateBlocksMC() {
  bool needNextLeft=true;
  //int lcurvalue, rcurvalue;
  Log::writeToLog("NLJoin", 1, "Generating MC blocks...");
  // check if input source is empty
  if (lBlockIn == NULL || rBlockIn == NULL) {
    // check if in bogus case? is this case bogus?
    //if (lBlockIn != NULL || rBlockIn != NULL) {
    //  throw new UnexpectedException("NLJoin: Should never happen???");
    //}
    if (blocks==NULL) {
      lBlockIn = l_src->getNextValBlock(l_predcol);
      lBlockInExtra = l_src->getNextValBlock(1-l_predcol);
      rBlockIn = r_src->getNextValBlock(r_predcol);
      rBlockInExtra = r_src->getNextValBlock(1-r_predcol);
      if (lBlockIn == NULL || rBlockIn == NULL) {
	//lBlockOut = rBlockOut = NULL;
	//svBlockOut = NULL;
	outRLEBlock=NULL;
	outBlockIter=NULL;
	return;
      }
      else {
	currLPair=lBlockIn->getNext();
	needNextLeft=false;
	//getNextLPairMC();
	//lBlockOut = new PosRLEBlock();
	outRLEBlock = new RLEBlock(false);
	outBlockIter = NULL;
      }

      // Right now, we need to copy Blocks because we can't pin them. Down the road, we can just pin them and all the below hacky code can be cleaned.
      if (!rBlockIn->isPosContiguous()) {
	throw new UnimplementedException("NLJoin: For the release, we only allow join on uncompressed data");
      }
      else {
	Log::writeToLog("NLJoin", 1, "Reading inner column");
	blocks = new Block*[1500000];
	blocks2 = new Block*[1500000];
	blocks3 = new BlockIter*[1500000];
	blocks4 = new BlockIter*[1500000];
	sblocks = new int*[1500000];
	sblocksizes = new int[1500000];
	int sp1,sp2,ep1,ep2;
	sp1 = rBlockIn->getStartPair()->position;
	sp2 = rBlockInExtra->getStartPair()->position;
	ep1 = rBlockIn->getEndPosition();
	ep2 = rBlockInExtra->getEndPosition();
	Block* tb1 = rBlockIn->getBlock();
	Block* tb2 = rBlockInExtra->getBlock();
	blocks[0] = tb1->clone(*tb1);
	blocks2[0] = tb2->clone(*tb2);
	blocks3[0] = blocks[0]->getIterator(sp1,ep1);
	blocks4[0] = blocks2[0]->getIterator(sp2,ep2);
	sblocks[0] = (int*)blocks3[0]->asArray();
	sblocksizes[0] = blocks3[0]->getSize();
	int i = 1;
	while ((rBlockIn=r_src->getNextValBlock(r_predcol))) {
	  rBlockInExtra = r_src->getNextValBlock(1-r_predcol);
	  tb1 = rBlockIn->getBlock();
	  tb2 = rBlockInExtra->getBlock();
	  sp1 = rBlockIn->getStartPair()->position;
	  sp2 = rBlockInExtra->getStartPair()->position;
	  ep1 = rBlockIn->getEndPosition();
	  ep2 = rBlockInExtra->getEndPosition();
	  blocks[i] = tb1->clone(*tb1);
	  blocks2[i] = tb2->clone(*tb2);
	  blocks3[i] = blocks[i]->getIterator(sp1,ep1);
	  blocks4[i] = blocks2[i]->getIterator(sp2,ep2);
	  sblocks[i] = (int*)blocks3[i]->asArray();
	  sblocksizes[i] = blocks3[i]->getSize();
	  //blocks[i] = rBlockIn->clone(*rBlockIn);
	  i++;
	}
	blocks[i]=NULL;
	blocks2[i]=NULL;
	blocks3[i]=NULL;
	blocks4[i]=NULL;
	sblocks[i] = NULL;
	Log::writeToLog("NLJoin", 1, "Done reading inner column...");
      }
      firstCall=false;
      rBlockIn = blocks3[0];
      rBlockInExtra = blocks4[0];
    }
    else {
      //lBlockOut = rBlockOut = NULL;
      //svBlockOut = NULL;
      outRLEBlock=NULL;
      outBlockIter=NULL;
      Log::writeToLog("NLJoin", 2, "Finished getting blocks from sources");
      return;
    }
  }
  ValPos* p;
  if (needNextLeft) {
    p = getNextLPairMC();
    if (!p) {
      //lBlockOut = rBlockOut = NULL;
      //svBlockOut = NULL;
      outRLEBlock=NULL;
      outBlockIter=NULL;
      Log::writeToLog("NLJoin", 2, "Finished getting blocks from sources");
      return;
    }
  }
  else
    p=currLPair;
  //Pair* pl, *pr;
  //int blockSize;
  //int out_idx = 0;
  //bool need_next_lhs = false;
  while (true) {
    //assume int type for now ... fix later
    assert(p->type == ValPos::INTTYPE);
    int pos = *(int*)p->value;
    int curmax=sblocksizes[0];
    int curpos=1;
    curblocki=0;
    while (pos > curmax) {
      curblocki++;
      curmax+=sblocksizes[curblocki];
      curpos+=sblocksizes[curblocki-1];
    }
    assert(*(int*)p->value == sblocks[curblocki][pos-curpos]);
    rBlockIn = blocks3[curblocki];
    rBlockInExtra = blocks4[curblocki];
    
	   /*    if (!rBlockIn->hasNext()) {
      rBlockIn->resetBlock();
      if (!(rBlockIn=blocks3[++curblocki])) {
	rBlockIn = blocks3[0];
	rBlockInExtra = blocks4[0];
	curblocki=0;
	Pair* p = getNextLPairMC();
	if (!p) {
	  //lBlockOut = rBlockOut = NULL;
	  //svBlockOut = NULL;
	  outRLEBlock=NULL;
	  outBlockIter=NULL;
	  Log::writeToLog("NLJoin", 2, "Finished getting blocks from sources");
	  return;
	}
      }
      else
	rBlockInExtra = blocks4[curblocki];
    }
    assert(rBlockIn->hasNext());
    // Compare the LHS tuple to the next tuple from the RHS.
    Pair* rhs = rBlockIn->getNext();
    int rhs_join_attr = rhs->value;
    int lhs_join_attr = currLPair->value;
    if (lhs_join_attr == rhs_join_attr) {*/
    int lblockSize;
      lblockSize = lBlockIn->getSize();
      //((PosRLEBlock*)lBlockOut)->setTriple(new RLETriple(lhs_join_attr,currLPair->position,lblockSize));
      outBlockIter = lBlockInExtra->getBlock()->getIterator(currLPair->position, currLPair->position+lblockSize-1);
      outRLEBlock->setTriple(new RLETriple(rBlockInExtra->getPairAtLoc(pos-rBlockIn->getStartPair()->position),m_currOutputPosition, lblockSize));
      //m_currOutputPosition += blockSize;
      m_currOutputPosition +=lblockSize;
      return;
	   //}
  }
}

// tuple join
// We interpet the r/l_pred_col fields as the 0-based index of the join attribute.
SBlock* NLJoin::getNextSValBlockEM(int colIndex_) {
	if (join_done) {
		return NULL;
	}
	bool need_next_lhs = true;
	// If this is the first call, then we do some first-time setup
	if (sblocks == NULL) {
		readSvalInnerTable();
		// We want to initialize our outblock, but to do that we need to
		// know the width of the output tuples. We've already read the right
		// operator tuples, so now we just need a left operator block so that
		// we can get its tuplewidth. The integer to getNextSValBlock has no meaning--
		// the streams only have one output.
		svlBlockIn = (SMultiBlock*) l_src->getNextSValBlock(0); 
		
		// Output tuples have width left + right - 1 (don't count the
		// join column twice). This value is in BYTES.
		out_tupwidth = ((SMultiBlock*)svlBlockIn)->getTupwidth() + 
							rightFirstBlock->getTupwidth() - sizeof(int);
		
		// We'd like our output block to be about PAGE_SIZE bytes, so we need to
		// choose the right value.
		int capacity = (int) ceil(PAGE_SIZE / (double) out_tupwidth);
		
		svBlockOut = new SMultiBlock(capacity, out_tupwidth);
		assert(capacity= svBlockOut->getCapacity());
		
		// Get the current LHS tuple
		currLHSTuple = svlBlockIn->getNext();
		need_next_lhs = false;
		currRightIndex=0;
	}

	// Now, we need to fill the svBlockOut block with valid join tuples. We're implementing
	// standard NL join, so we're going to pick a tuple from the left iterator and then
	// compare it to every single tuple in all of the sblocks. We're going to do this
	// until we run out of left tuples or we fill the svBlockOut block.
	svBlockOut->resetBlock();
	
	// Try to fill the out block to capacity
	int out_idx = 0;
	int capacity = svBlockOut->getCapacity();
	int rtuplesize = rightFirstBlock->getTupwidth() / sizeof(int);

	while(svBlockOut->getNElements() < capacity) {

	  /*		
		// If the current RHS block has no more tuples, try getting the next one
		//int* rhsblock = sblocks[curr_rhs_block_idx];
		if (!(currRightIndex<sblocksizes[curr_rhs_block_idx])) {
		  //if (!rhsblock->hasNext()) {
//		if (!sblocks[curr_rhs_block_idx]->hasNext()) {
		  //((SMultiBlock*)sblocks[curr_rhs_block_idx])->resetIterator();
			++curr_rhs_block_idx;
			currRightIndex=0;
			// If there are no more RHS blocks, then we're done with the
			// join for this LHS tuple. 
			if (sblocks[curr_rhs_block_idx] == NULL) {
				// Reset the RHS
				curr_rhs_block_idx = 0;
				need_next_lhs = true;		
			}
		}
	  */
		// If we need to fetch the next LHS tuple, then we'll go ahead and retrieve it.
		// We might not if we're still in the middle of the RHS.
		if (need_next_lhs) {
			if (!svlBlockIn->hasNext()) {
				svlBlockIn = (SMultiBlock*) l_src->getNextSValBlock(0);
				// If there are no more LHS blocks, then we're done with the join.
				if (svlBlockIn == NULL || !svlBlockIn->hasNext()) {
					join_done = true;
					return svBlockOut;
				}
			}
			// Fetch the next tuple--we know it must be available now.
			currLHSTuple = svlBlockIn->getNext();
		}
		
		// If there's nothing in the svBlockOut, then this tuple defines
		// the start position
		if (svBlockOut->getNElements() == 0) {
			svBlockOut->setStartPosition(currLHSTuple->position);
		}
		
		int lhs_join_attr = ((int*)currLHSTuple->value)[l_predcol];
		int pos = lhs_join_attr;
		int curmax=sblocksizes[0];
		int curpos=1;
		curr_rhs_block_idx=0;
		while (pos > curmax) {
		  curr_rhs_block_idx++;
		  curmax+=sblocksizes[curr_rhs_block_idx];
		  curpos+=sblocksizes[curr_rhs_block_idx-1];
		}
		
		int* rhsTuple = sblocks[curr_rhs_block_idx] + (rtuplesize*(pos-curpos));
		assert(pos == rhsTuple[r_predcol]);
		// Compare the LHS tuple to the next tuple from the RHS.
		//int* rhsTuple = sblocks[curr_rhs_block_idx] + (rtuplesize*currRightIndex);
		//currRightIndex++;
		//int rhs_join_attr = rhsTuple[r_predcol];
		//int lhs_join_attr = ((int*)currLHSTuple->value)[l_predcol];

		// This is a join match
		//if (lhs_join_attr == rhs_join_attr) {
			char* joinval = new char[out_tupwidth];
			// Copy in the LHS tuple
			memcpy(joinval, currLHSTuple->value, svlBlockIn->getTupwidth());	
			// Copy in the RHS tuple up to (but not including) the join attr
			memcpy(joinval + svlBlockIn->getTupwidth(), (byte*)rhsTuple, 
								r_predcol * sizeof(int));
			// Copy in the RHS tuple from one beyond the join attr to the end
			memcpy(joinval + svlBlockIn->getTupwidth() + r_predcol * sizeof(int),
				((byte*)rhsTuple) + ((r_predcol+1) * sizeof(int)),
						rtuplesize*sizeof(int) -
							 ((r_predcol+1)*sizeof(int)));
			svBlockOut->setValueAtPos(joinval, out_idx);
			//cerr << "Set at " << out_idx << " val " << ((int*)joinval)[0] << endl;
			++out_idx;
			need_next_lhs=true;
			//}
	}
	return svBlockOut;
}



// ASSUMPTIONS:
//  positions across both block streams line up in rows

// INVARIANT:
// lBlockIn, rBlockIn is always set to next pair to be read
void NLJoin::generateBlocks() {
  if (!doEM) {
    if (lBlockIn==NULL) {
	  lBlockOut = new PosRLEBlock();
	  rBlockOut = new PosBasicBlock(true);
    }
    lBlockIn = l_src->getNextValBlock(l_predcol);
    if (!lBlockIn) {
      lBlockOut = rBlockOut = NULL;
      return;
    }
    ValPos* p = lBlockIn->getNext();
    int lblockSize = lBlockIn->getSize();
    ((PosRLEBlock*)lBlockOut)->setTriple(new RLETriple(p,p->position,lblockSize));
    //I don't think this code ever gets called.
    //just to be sure, I'm doing an assert(false);
    //feel free to remove if it actually gets called
    assert(false);
    assert(p->type == ValPos::INTTYPE);
    ((PosBasicBlock*)rBlockOut)->setPosition(*(int*)p->value);
    rBlockOut->setNumOcurrences(lblockSize);
    return;
  }
  //int lcurvalue, rcurvalue;
  Log::writeToLog("NLJoin", 1, "Generating blocks...");
  // check if input source is empty
  if (lBlockIn == NULL || rBlockIn == NULL) {
    // check if in bogus case? is this case bogus?
    //if (lBlockIn != NULL || rBlockIn != NULL) {
    //  throw new UnexpectedException("NLJoin: Should never happen???");
    //}
    if (firstCall) {
      lBlockIn = l_src->getNextValBlock(l_predcol);
      rBlockIn = r_src->getNextValBlock(r_predcol);
      if (lBlockIn == NULL || rBlockIn == NULL) {
	lBlockOut = rBlockOut = NULL;
	return;
      }
      else {
	getNextLPair();
	initOutputBlocks();
      }

      // Right now, we need to copy Blocks because we can't pin them. Down the road, we can just pin them and all the below hacky code can be cleaned.
      if (!rBlockIn->isPosSorted()) {
	throw new UnimplementedException("NLJoin: For now join only works for pos sorted data");
      }
      else {
	Log::writeToLog("NLJoin", 1, "Reading inner column");
	blocks = new Block*[1500000];
	blocks[0] = rBlockIn->clone(*rBlockIn);
	int i = 1;
	while ((rBlockIn=r_src->getNextValBlock(r_predcol))) {
	  //blocks[i] = new BasicBlock(((BasicBlock*)rBlockIn)&);
	  blocks[i] = rBlockIn->clone(*rBlockIn);
	  i++;
	}
	blocks[i]=NULL;
	Log::writeToLog("NLJoin", 1, "Done reading inner column...");
      }
      firstCall=false;
      rBlockIn = blocks[0];
    }
    else {
      lBlockOut = rBlockOut = NULL;
      Log::writeToLog("NLJoin", 2, "Finished getting blocks from sources");
      return;
    }
  } 
  
  ValPos* pl, *pr;
  int blockSize;
  if ((lBlockIn->getSize() == 1) && (!(dynamic_cast<RLEBlock*>(lBlockIn)))) {
    if (lBlockIn->isPosContiguous()) {
      if (lBlockIn->isOneValue()) {
    //if  (!lBlockIn->isValueSorted()) { DOESN'T MATTER IF SORTED OR NOT WITH ONE VALUE
	  if (rBlockIn->isOneValue()) {
	    //if  (rBlockIn->isValueSorted()) { DOESN'T MATTER IF SORTED OR NOT WITH ONE VALUE
	    if (rBlockIn->getSize() > 1) {
	      if (rBlockIn->isPosContiguous()) {
		
		throw new UnimplementedException("NLJoin: For the release, we only allow join on uncompressed data");
		
	      }
	    
	      else if (!rBlockIn->isPosContiguous()) {
		
		throw new UnimplementedException("NLJoin: For the release, we only allow join on uncompressed data");
		
	      }
	    }
	    else { //size is one
	      pl = lBlockIn->getStartPair();
	      pr = rBlockIn->getStartPair();
	      //lcurvalue = pl->value;
	      //rcurvalue = pr->value;
	      //if (lcurvalue==rcurvalue) {
	      if (*pl == pr) {
		blockSize = rBlockIn->getSize();
		//cout << "joining: " << lcurvalue << endl;
		//cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		//cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		((PosBasicBlock*)rBlockOut)->setPosition(rBlockIn->getStartPair()->position);
		m_currOutputPosition += blockSize;
		if (!(rBlockIn=blocks[++curblocki])) {
		  rBlockIn = blocks[0];
		  curblocki=0;
		  lBlockIn=l_src->getNextValBlock(l_predcol);
		}
		else
		  lBlockIn->resetBlock();
		return;
	      }
	      while ((rBlockIn=blocks[++curblocki])) {
		pr = rBlockIn->getStartPair();
		//rcurvalue = rBlockIn->getStartPair()->value;
		//if (lcurvalue==rcurvalue) {
		if (*pl == pr) {
		  blockSize = rBlockIn->getSize();
		  //cout << "joining: " << lcurvalue << endl;
		  //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		  //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		  ((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		  ((PosBasicBlock*)rBlockOut)->setPosition(rBlockIn->getStartPair()->position);
		  m_currOutputPosition += blockSize;
		  if (!(rBlockIn=blocks[++curblocki])) {
		    rBlockIn = blocks[0];
		    curblocki=0;
		    lBlockIn=l_src->getNextValBlock(l_predcol);
		  }
		  else
		    lBlockIn->resetBlock();
		  return;
		}
	      }
	      rBlockIn = blocks[0];
	      curblocki=0;
	      while ((lBlockIn=l_src->getNextValBlock(l_predcol))) {
		//lcurvalue = lBlockIn->getStartPair()->value;
		//rcurvalue = rBlockIn->getStartPair()->value;
		//if (lcurvalue==rcurvalue) {
		pl = lBlockIn->getStartPair();
		pr = rBlockIn->getStartPair();
		if (*pl == pr) {
		  blockSize = rBlockIn->getSize();
		  //cout << "joining: " << lcurvalue << endl;
		  //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		  //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		  ((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		  ((PosBasicBlock*)rBlockOut)->setPosition(rBlockIn->getStartPair()->position);
		  m_currOutputPosition += blockSize;
		  if (!(rBlockIn=blocks[++curblocki])) {
		    rBlockIn = blocks[0];
		    curblocki=0;
		    lBlockIn=l_src->getNextValBlock(l_predcol);
		  }
		  else
		    lBlockIn->resetBlock();
		  return;
		}
		while ((rBlockIn=blocks[++curblocki])) {
		  pr = rBlockIn->getStartPair();
		  //rcurvalue = rBlockIn->getStartPair()->value;
		  //if (lcurvalue==rcurvalue) {
		  if (*pl == pr) {
		    blockSize = rBlockIn->getSize();
		    //cout << "joining: " << lcurvalue << endl;
		    //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		    //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		    ((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		    ((PosBasicBlock*)rBlockOut)->setPosition(rBlockIn->getStartPair()->position);
		    m_currOutputPosition += blockSize;
		    if (!(rBlockIn=blocks[++curblocki])) {
		      rBlockIn = blocks[0];
		      curblocki=0;
		      lBlockIn=l_src->getNextValBlock(l_predcol);
		    }
		    else
		      lBlockIn->resetBlock();
		    return;
		  }
		}
		rBlockIn = blocks[0];
		curblocki=0;
	      }
	      
	      if (lBlockIn == NULL) 
		lBlockOut = rBlockOut = NULL;
	      
	    } 
	  }
	  else {
	    pl = lBlockIn->getStartPair();
	    pr = rBlockIn->getNext();
	    //lcurvalue = pl->value;
	    //rcurvalue = pr->value;
	    //if (lcurvalue==rcurvalue) {
	    if (*pl == pr) {
	      blockSize = rBlockIn->getSize();
		//cout << "joining: " << lcurvalue << endl;
		//cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		//cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
	      ((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
	      ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
	      //m_currOutputPosition += blockSize;
	      m_currOutputPosition ++;
	      if (!rBlockIn->hasNext()) {
		rBlockIn->resetBlock();
		if (!(rBlockIn=blocks[++curblocki])) {
		  rBlockIn = blocks[0];
		  curblocki=0;
		  lBlockIn=l_src->getNextValBlock(l_predcol);
		}
	      }
	      return;
	    }
	    while (rBlockIn) {
	      while  ((pr = rBlockIn->getNext())) {
	      //while ((rBlockIn=blocks[++curblocki])) {
	      //rcurvalue = pr->value;
	      //if (lcurvalue==rcurvalue) {
	      if (*pl == pr) {
		blockSize = rBlockIn->getSize();
		//cout << "joining: " << lcurvalue << endl;
		//cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		//cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		//m_currOutputPosition += blockSize;
		m_currOutputPosition ++;
		if (!rBlockIn->hasNext()) {
		  rBlockIn->resetBlock();
		  if (!(rBlockIn=blocks[++curblocki])) {
		    rBlockIn = blocks[0];
		    curblocki=0;
		    lBlockIn=l_src->getNextValBlock(l_predcol);
		  }
		}
		return;
	      }
	      }
	      rBlockIn->resetBlock();
	      rBlockIn=blocks[++curblocki];
	    }
	    rBlockIn = blocks[0];
	    curblocki=0;
	    while ((lBlockIn=l_src->getNextValBlock(l_predcol))) {
	      //lcurvalue = (pl=lBlockIn->getStartPair())->value;
	      pl=lBlockIn->getStartPair();
		pr = rBlockIn->getNext();
		//rcurvalue = pr->value;
		//if (lcurvalue==rcurvalue) {
		if (*pl == pr) {
		  blockSize = rBlockIn->getSize();
		  //cout << "joining: " << lcurvalue << endl;
		  //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		  //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		  ((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		  ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		  //m_currOutputPosition += blockSize;
		  m_currOutputPosition ++;
		  if (!rBlockIn->hasNext()) {
		    rBlockIn->resetBlock();
		    if (!(rBlockIn=blocks[++curblocki])) {
		      rBlockIn = blocks[0];
		      curblocki=0;
		      lBlockIn=l_src->getNextValBlock(l_predcol);
		    }
		    else
		      lBlockIn->resetBlock();
		  }
		  return;
		}
		while (rBlockIn) {
		  while  ((pr = rBlockIn->getNext())) {
		  //while ((rBlockIn=blocks[++curblocki])) {
		  //rcurvalue = pr->value;
		  //if (lcurvalue==rcurvalue) {
		  if (*pl == pr) {
		    blockSize = rBlockIn->getSize();
		    //cout << "joining: " << lcurvalue << endl;
		    //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		    //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		    ((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		    ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		    //m_currOutputPosition += blockSize;
		    m_currOutputPosition ++;
		    if (!rBlockIn->hasNext()) {
		      rBlockIn->resetBlock();
		      if (!(rBlockIn=blocks[++curblocki])) {
			rBlockIn = blocks[0];
			curblocki=0;
			lBlockIn=l_src->getNextValBlock(l_predcol);
		      }
		      else
			lBlockIn->resetBlock();
		    }
		    return;
		  }
		  }
		  rBlockIn->resetBlock();
		  rBlockIn=blocks[++curblocki];
		}
		rBlockIn = blocks[0];
		curblocki=0;
	      }
	      
	      if (lBlockIn == NULL) 
		lBlockOut = rBlockOut = NULL;
	  }
	  
	}
         }
      }




  else {

    
    //throw new UnimplementedException("NLJoin: For the release, we only allow join on uncompressed data");
    
    int lblockSize;
      if (lBlockIn->isPosContiguous()) {
	if (lBlockIn->isOneValue() == 1) {

	  if (rBlockIn->isOneValue()) {
	    /*  //if  (rBlockIn->isValueSorted()) { DOESN'T MATTER IF SORTED OR NOT WITH ONE VALUE
	    if (rBlockIn->getSize() > 1) {
	      if (rBlockIn->isPosContiguous()) {
		pl = lBlockIn->getStartPair();
		pr = rBlockIn->getStartPair();
		lcurvalue = pl->value;
		rcurvalue = pr->value;
		if (lcurvalue==rcurvalue) {
		  blockSize = rBlockIn->getSize();
		  lblockSize = lBlockIn->getSize();
		  cout << "make l triple " << pl->position << " " << m_currOutputPosition << " " << blockSize << endl;
		  cout << "..." << endl;
		    cout << "make l triple " << (pl->position + lblockSize - 1) << " " << (m_currOutputPosition + (blockSize * (lblockSize-1)) - 1) << " " << blockSize << endl;
		  cout << "make variable block (" << lblockSize << " times) " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << rBlockIn->getStartPair()->position + blockSize - 1 << "," << m_currOutputPosition + blockSize - 1 <<  endl;
		  m_currOutputPosition += (blockSize * lblockSize);
		  if (!(rBlockIn=blocks[++curblocki])) {
		    rBlockIn = blocks[0];
		    curblocki=0;
		    lBlockIn=l_src->getNextValBlock(l_predcol);
		  }
		  else
		    lBlockIn->resetBlock();
		  return;
		}
		while ((rBlockIn=blocks[++curblocki])) {
		  rcurvalue = rBlockIn->getStartPair()->value;
		  if (lcurvalue==rcurvalue) {
		    blockSize = rBlockIn->getSize();
		  cout << "make l triple " << pl->position << " " << m_currOutputPosition << " " << blockSize << endl;
		  cout << "..." << endl;
		    cout << "make l triple " << (pl->position + lblockSize - 1) << " " << (m_currOutputPosition + (blockSize * (lblockSize-1)) - 1) << " " << blockSize << endl;
		  cout << "make variable block (" << lblockSize << " times) " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << rBlockIn->getStartPair()->position + blockSize - 1 << "," << m_currOutputPosition + blockSize - 1 <<  endl;
		    m_currOutputPosition += blockSize;
		    if (!(rBlockIn=blocks[++curblocki])) {
		      rBlockIn = blocks[0];
		      curblocki=0;
		      lBlockIn=l_src->getNextValBlock(l_predcol);
		    }
		    else
		      lBlockIn->resetBlock();
		    return;
		  }
		}
		rBlockIn = blocks[0];
		curblocki=0;
		while ((lBlockIn=l_src->getNextValBlock(l_predcol))) {
		  lcurvalue = lBlockIn->getStartPair()->value;
		  rcurvalue = rBlockIn->getStartPair()->value;
		  if (lcurvalue==rcurvalue) {
		    blockSize = rBlockIn->getSize();
		  cout << "make l triple " << pl->position << " " << m_currOutputPosition << " " << blockSize << endl;
		  cout << "..." << endl;
		    cout << "make l triple " << (pl->position + lblockSize - 1) << " " << (m_currOutputPosition + (blockSize * (lblockSize-1)) - 1) << " " << blockSize << endl;
		  cout << "make variable block (" << lblockSize << " times) " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << rBlockIn->getStartPair()->position + blockSize - 1 << "," << m_currOutputPosition + blockSize - 1 <<  endl;
		    m_currOutputPosition += blockSize;
		    if (!(rBlockIn=blocks[++curblocki])) {
		      rBlockIn = blocks[0];
		      curblocki=0;
		      lBlockIn=l_src->getNextValBlock(l_predcol);
		    }
		    else
		      lBlockIn->resetBlock();
		    return;
		  }
		  while ((rBlockIn=blocks[++curblocki])) {
		    rcurvalue = rBlockIn->getStartPair()->value;
		    if (lcurvalue==rcurvalue) {
		      blockSize = rBlockIn->getSize();
		  cout << "make l triple " << pl->position << " " << m_currOutputPosition << " " << blockSize << endl;
		  cout << "..." << endl;
		    cout << "make l triple " << (pl->position + lblockSize - 1) << " " << (m_currOutputPosition + (blockSize * (lblockSize-1)) - 1) << " " << blockSize << endl;
		  cout << "make variable block (" << lblockSize << " times) " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << rBlockIn->getStartPair()->position + blockSize - 1 << "," << m_currOutputPosition + blockSize - 1 <<  endl;
		      m_currOutputPosition += blockSize;
		      if (!(rBlockIn=blocks[++curblocki])) {
			rBlockIn = blocks[0];
			curblocki=0;
			lBlockIn=l_src->getNextValBlock(l_predcol);
		      }
		      else
			lBlockIn->resetBlock();
		      return;
		    }
		  }
		  rBlockIn = blocks[0];
		  curblocki=0;
		  
		}
		
		
		Log::writeToLog("NLJoin", 0, "Not coded with RLE");
	      }
	    
	      else if (!rBlockIn->isPosContiguous()) {
		pl = lBlockIn->getStartPair();
		while (pl->value > 199) {
		  lBlockIn=l_src->getNextValBlock(l_predcol);
		  pl = lBlockIn->getStartPair();
		}
		//NEED TO CONVERT REG TYPE2 BLOCK to POSBLOCK HERE
		//rBlockOut=blocks[curblocki];
		if (curblocki != indicies[pl->value][1]) {
		  curblocki++;
		  rBlockIn = blocks[curblocki];
		  return;
		}
		else {
		  while ((lBlockIn=l_src->getNextValBlock(l_predcol))) {
		    lcurvalue = lBlockIn->getStartPair()->value;
		    if (lcurvalue > 199) continue;
		    if ((curblocki = indicies[lcurvalue][0]) != -1) {
		      rBlockIn = blocks[curblocki];
		      blockSize = rBlockIn->getSize();
		      cout << "make position block " << pl->position << " " << m_currOutputPosition << " " << blockSize << endl;
		      m_currOutputPosition += blockSize + 1;
		      return;
		    }
		  }
		}
		
		Log::writeToLog("NLJoin", 0, "Not coded with BVE");
	      }
	    }
	    else { //size is one
	      pl = lBlockIn->getStartPair();
	      pr = rBlockIn->getStartPair();
	      lcurvalue = pl->value;
	      rcurvalue = pr->value;
	      if (lcurvalue==rcurvalue) {
		blockSize = rBlockIn->getSize();
		cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		//((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		//((PosBasicBlock*)rBlockOut)->setPosition(rBlockIn->getStartPair()->position);
		m_currOutputPosition += blockSize;
		if (!(rBlockIn=blocks[++curblocki])) {
		  rBlockIn = blocks[0];
		  curblocki=0;
		  lBlockIn=l_src->getNextValBlock(l_predcol);
		}
		else
		  lBlockIn->resetBlock();
		return;
	      }
	      while ((rBlockIn=blocks[++curblocki])) {
		rcurvalue = rBlockIn->getStartPair()->value;
		if (lcurvalue==rcurvalue) {
		  blockSize = rBlockIn->getSize();
		  cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		  cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		  m_currOutputPosition += blockSize;
		  if (!(rBlockIn=blocks[++curblocki])) {
		    rBlockIn = blocks[0];
		    curblocki=0;
		    lBlockIn=l_src->getNextValBlock(l_predcol);
		  }
		  else
		    lBlockIn->resetBlock();
		  return;
		}
	      }
	      rBlockIn = blocks[0];
	      curblocki=0;
	      while ((lBlockIn=l_src->getNextValBlock(l_predcol))) {
		lcurvalue = lBlockIn->getStartPair()->value;
		rcurvalue = rBlockIn->getStartPair()->value;
		if (lcurvalue==rcurvalue) {
		  blockSize = rBlockIn->getSize();
		  cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		  cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		  m_currOutputPosition += blockSize;
		  if (!(rBlockIn=blocks[++curblocki])) {
		    rBlockIn = blocks[0];
		    curblocki=0;
		    lBlockIn=l_src->getNextValBlock(l_predcol);
		  }
		  else
		    lBlockIn->resetBlock();
		  return;
		}
		while ((rBlockIn=blocks[++curblocki])) {
		  rcurvalue = rBlockIn->getStartPair()->value;
		  if (lcurvalue==rcurvalue) {
		    blockSize = rBlockIn->getSize();
		  cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		  cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		    m_currOutputPosition += blockSize;
		    if (!(rBlockIn=blocks[++curblocki])) {
		      rBlockIn = blocks[0];
		      curblocki=0;
		      lBlockIn=l_src->getNextValBlock(l_predcol);
		    }
		    else
		      lBlockIn->resetBlock();
		    return;
		  }
		}
		rBlockIn = blocks[0];
		curblocki=0;
	      }
	      
	      }*/
	  }
	  else { //rblockin more than 1 value no point asking about size or poscontig
	    //RLE with Multi will get in this case
	    pl = lBlockIn->getStartPair();
	    pr = rBlockIn->getNext();
	    //lcurvalue = pl->value;
	    //rcurvalue = pr->value;
	    //if (lcurvalue==rcurvalue) {
	    if (*pl == pr) {
	      blockSize = rBlockIn->getSize();
	      lblockSize = lBlockIn->getSize();
		//cout << "joining: " << lcurvalue << endl;
		//cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		//cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
	      ((PosRLEBlock*)lBlockOut)->setTriple(new RLETriple(pl,pl->position,lblockSize));
	      ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
	      rBlockOut->setNumOcurrences(lblockSize);
	      //m_currOutputPosition += blockSize;
	      m_currOutputPosition ++;
	      if (!rBlockIn->hasNext()) {
		rBlockIn->resetBlock();
		if (!(rBlockIn=blocks[++curblocki])) {
		  rBlockIn = blocks[0];
		  curblocki=0;
		  lBlockIn=l_src->getNextValBlock(l_predcol);
		}
	      }
	      return;
	    }
	    while (rBlockIn) {
	      while  ((pr = rBlockIn->getNext())) {
	      //while ((rBlockIn=blocks[++curblocki])) {
	      //rcurvalue = pr->value;
	      //if (lcurvalue==rcurvalue) {
		if (*pl == pr) {
		blockSize = rBlockIn->getSize();
		lblockSize = lBlockIn->getSize();
		//cout << "joining: " << lcurvalue << endl;
		//cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		//cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		((PosRLEBlock*)lBlockOut)->setTriple(new RLETriple(pl,pl->position,lblockSize));
		((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		rBlockOut->setNumOcurrences(lblockSize);
		//m_currOutputPosition += blockSize;
		m_currOutputPosition ++;
		if (!rBlockIn->hasNext()) {
		  rBlockIn->resetBlock();
		  if (!(rBlockIn=blocks[++curblocki])) {
		    rBlockIn = blocks[0];
		    curblocki=0;
		    lBlockIn=l_src->getNextValBlock(l_predcol);
		  }
		}
		return;
	      }
	      }
	      rBlockIn->resetBlock();
	      rBlockIn=blocks[++curblocki];
	    }
	    rBlockIn = blocks[0];
	    curblocki=0;
	    while ((lBlockIn=l_src->getNextValBlock(l_predcol))) {
	      //lcurvalue = (pl=lBlockIn->getStartPair())->value;
	      pl=lBlockIn->getStartPair();
		pr = rBlockIn->getNext();
		//rcurvalue = pr->value;
		//if (lcurvalue==rcurvalue) {
		if (*pl == pr) {
		  blockSize = rBlockIn->getSize();
		  lblockSize = lBlockIn->getSize();
		  //cout << "joining: " << lcurvalue << endl;
		  //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		  //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		  ((PosRLEBlock*)lBlockOut)->setTriple(new RLETriple(pl,pl->position,lblockSize));
		  ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		  rBlockOut->setNumOcurrences(lblockSize);
		  //m_currOutputPosition += blockSize;
		  m_currOutputPosition ++;
		  if (!rBlockIn->hasNext()) {
		    rBlockIn->resetBlock();
		    if (!(rBlockIn=blocks[++curblocki])) {
		      rBlockIn = blocks[0];
		      curblocki=0;
		      lBlockIn=l_src->getNextValBlock(l_predcol);
		    }
		    else
		      lBlockIn->resetBlock();
		  }
		  return;
		}
		while (rBlockIn) {
		  while  ((pr = rBlockIn->getNext())) {
		  //while ((rBlockIn=blocks[++curblocki])) {
		  //rcurvalue = pr->value;
		  //if (lcurvalue==rcurvalue) {
		    if (*pl == pr) {
		    blockSize = rBlockIn->getSize();
		    lblockSize = lBlockIn->getSize();
		    //cout << "joining: " << lcurvalue << endl;
		    //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		    //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		    ((PosRLEBlock*)lBlockOut)->setTriple(new RLETriple(pl,pl->position,lblockSize));
		    ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		    rBlockOut->setNumOcurrences(lblockSize);
		    //m_currOutputPosition += blockSize;
		    m_currOutputPosition ++;
		    if (!rBlockIn->hasNext()) {
		      rBlockIn->resetBlock();
		      if (!(rBlockIn=blocks[++curblocki])) {
			rBlockIn = blocks[0];
			curblocki=0;
			lBlockIn=l_src->getNextValBlock(l_predcol);
		      }
		      else
			lBlockIn->resetBlock();
		    }
		    return;
		  }
		  }
		  rBlockIn->resetBlock();
		  rBlockIn=blocks[++curblocki];
		}
		rBlockIn = blocks[0];
		curblocki=0;
	      }
	      
	      if (lBlockIn == NULL) 
		lBlockOut = rBlockOut = NULL;
	  }
	  
	}
	else {
	  //e.g. MULTI-MULTI
	  if (!rBlockIn->isOneValue()) {
	    //if (rBlockIn->getSize() > 1) {
		pl = getCurrLPair();
		pr = rBlockIn->getNext();
		//lcurvalue = pl->value;
		//rcurvalue = pr->value;
		//if (lcurvalue==rcurvalue) {
		  if (*pl == pr) {
		  //cout << "joining: " << lcurvalue << endl;
		  //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		  //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		  ((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		  ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		  //m_currOutputPosition += blockSize;
		  m_currOutputPosition ++;
		  if (!rBlockIn->hasNext()) {
		    rBlockIn->resetBlock();
		    if (!(rBlockIn=blocks[++curblocki])) {
		      rBlockIn = blocks[0];
		      curblocki=0;
		      getNextLPair();
		    }
		    //else
		    //  lBlockIn->resetBlock();
		  }
		  return;
		}
		if (!rBlockIn->hasNext()) {
		  rBlockIn->resetBlock();
		  rBlockIn=blocks[++curblocki];
		}
		while (rBlockIn) {
		  while  (rBlockIn->hasNext()) {
		    pr = rBlockIn->getNext();
		    //while ((rBlockIn=blocks[++curblocki])) {
		    //rcurvalue = pr->value;
		    //if (lcurvalue==rcurvalue) {
		    if (*pl == pr) {
		      //cout << "joining: " << lcurvalue << endl;
		      //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		      //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		      ((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		      ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		      //m_currOutputPosition += blockSize;
		      m_currOutputPosition ++;
		      if (!rBlockIn->hasNext()) {
			rBlockIn->resetBlock();
			if (!(rBlockIn=blocks[++curblocki])) {
			  rBlockIn = blocks[0];
			  curblocki=0;
			  getNextLPair();
			}
		      }
		      return;
		    }
		  }
		  rBlockIn->resetBlock();
		  rBlockIn=blocks[++curblocki];
		}
		rBlockIn = blocks[0];
		curblocki=0;
		while ((pl=getNextLPair())) {
		  //lcurvalue = pl->value;
		  pr = rBlockIn->getNext();
		  //rcurvalue = pr->value;
		  //if (lcurvalue==rcurvalue) {
		  if (*pl == pr) {
		    //cout << "joining: " << lcurvalue << endl;
		    //cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
		    //cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
		    ((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
		    ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		    //m_currOutputPosition += blockSize;
		    m_currOutputPosition ++;
		    if (!rBlockIn->hasNext()) {
		      rBlockIn->resetBlock();
		      if (!(rBlockIn=blocks[++curblocki])) {
			rBlockIn = blocks[0];
			curblocki=0;
			getNextLPair();
		      }
		    }
		    return;
		  }
		  if (!rBlockIn->hasNext()) {
		    rBlockIn->resetBlock();
		    rBlockIn=blocks[++curblocki];
		  }
		  while (rBlockIn) {
		    while  (rBlockIn->hasNext()) {
		      pr = rBlockIn->getNext();
		      //while ((rBlockIn=blocks[++curblocki])) {
		      //rcurvalue = pr->value;
		      //if (lcurvalue==rcurvalue) {
		      if (*pl == pr) {
			//cout << "joining: " << lcurvalue << endl;
			//cout << "make position " << pl->position << " " << m_currOutputPosition << endl;
			//cout << "make position " << rBlockIn->getStartPair()->position << "," << m_currOutputPosition << " ... " << endl;
			((PosBasicBlock*)lBlockOut)->setPosition(pl->position);
			((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
			//m_currOutputPosition += blockSize;
			m_currOutputPosition ++;
			if (!rBlockIn->hasNext()) {
			  rBlockIn->resetBlock();
			  if (!(rBlockIn=blocks[++curblocki])) {
			    rBlockIn = blocks[0];
			    curblocki=0;
			    getNextLPair();
			  }
			}
			return;
		      }
		    }
		    rBlockIn->resetBlock();
		    rBlockIn=blocks[++curblocki];
		  }
		  rBlockIn = blocks[0];
		  curblocki=0;
		}
		
		if (lBlockIn == NULL) 
		  lBlockOut = rBlockOut = NULL;
		//}
	  }
	  
	}
      }
      else {
	  if (!rBlockIn->isOneValue()) {
	    //Type2 with Multi will get in this case
	    pl = lBlockIn->getStartPair();
	    pr = rBlockIn->getNext();
	    //lcurvalue = pl->value;
	    //rcurvalue = pr->value;
	    //if (lcurvalue==rcurvalue) {
	    if (*pl == pr) {
	      //shouldn't have to do this ... fix later
	      lblockSize = ((Type2Block*)lBlockIn)->getSize();
	      //byte* tempbfr = ((Type2Block*)lBlockIn)->getBuffer();
	      //Type2Header* header=((Type2Header*) tempbfr);
	      ((PosType2Block*)lBlockOut)->setBlock((Type2Block*)lBlockIn);
	      ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
	      rBlockOut->setNumOcurrences(lblockSize);
	      //m_currOutputPosition += blockSize;
	      m_currOutputPosition ++;
	      if (!rBlockIn->hasNext()) {
		rBlockIn->resetBlock();
		if (!(rBlockIn=blocks[++curblocki])) {
		  rBlockIn = blocks[0];
		  curblocki=0;
		  lBlockIn=l_src->getNextValBlock(l_predcol);
		}
	      }
	      return;
	    }
	    while (rBlockIn) {
	      while  ((pr = rBlockIn->getNext())) {
	      //while ((rBlockIn=blocks[++curblocki])) {
	      //rcurvalue = pr->value;
	      //if (lcurvalue==rcurvalue) {
		if (*pl == pr) {
		//shouldn't have to do this ... fix later
		lblockSize = ((Type2Block*)lBlockIn)->getSize();
		//byte* tempbfr = ((Type2Block*)lBlockIn)->getBuffer();
		//Type2Header* header=((Type2Header*) tempbfr);
		((PosType2Block*)lBlockOut)->setBlock((Type2Block*)lBlockIn);
		((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		rBlockOut->setNumOcurrences(lblockSize);
		//m_currOutputPosition += blockSize;
		m_currOutputPosition ++;
		if (!rBlockIn->hasNext()) {
		  rBlockIn->resetBlock();
		  if (!(rBlockIn=blocks[++curblocki])) {
		    rBlockIn = blocks[0];
		    curblocki=0;
		    lBlockIn=l_src->getNextValBlock(l_predcol);
		  }
		}
		return;
	      }
	      }
	      rBlockIn->resetBlock();
	      rBlockIn=blocks[++curblocki];
	    }
	    rBlockIn = blocks[0];
	    curblocki=0;
	    while ((lBlockIn=l_src->getNextValBlock(l_predcol))) {
	      //lcurvalue = (pl=lBlockIn->getStartPair())->value;
		pl=lBlockIn->getStartPair();
		pr = rBlockIn->getNext();
		//rcurvalue = pr->value;
		//if (lcurvalue==rcurvalue) {
		if (*pl == pr) {
		  //shouldn't have to do this ... fix later
		  lblockSize = ((Type2Block*)lBlockIn)->getSize();
		  //byte* tempbfr = ((Type2Block*)lBlockIn)->getBuffer();
		  //Type2Header* header=((Type2Header*) tempbfr);
		  ((PosType2Block*)lBlockOut)->setBlock((Type2Block*)lBlockIn);
		  ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		  rBlockOut->setNumOcurrences(lblockSize);
		  //m_currOutputPosition += blockSize;
		  m_currOutputPosition ++;
		  if (!rBlockIn->hasNext()) {
		    rBlockIn->resetBlock();
		    if (!(rBlockIn=blocks[++curblocki])) {
		      rBlockIn = blocks[0];
		      curblocki=0;
		      lBlockIn=l_src->getNextValBlock(l_predcol);
		    }
		    else
		      lBlockIn->resetBlock();
		  }
		  return;
		}
		while (rBlockIn) {
		  while  ((pr = rBlockIn->getNext())) {
		  //while ((rBlockIn=blocks[++curblocki])) {
		  //rcurvalue = pr->value;
		  //if (lcurvalue==rcurvalue) {
		  if (*pl == pr) {
		    //shouldn't have to do this ... fix later
		    lblockSize = ((Type2Block*)lBlockIn)->getSize();
		    //byte* tempbfr = ((Type2Block*)lBlockIn)->getBuffer();
		    //Type2Header* header=((Type2Header*) tempbfr);
		    ((PosType2Block*)lBlockOut)->setBlock((Type2Block*)lBlockIn);
		    //((PosType2Block*)lBlockOut)->setBuffer(header->startPos, header->numValues, tempbfr+3*sizeof(int));
		    ((PosBasicBlock*)rBlockOut)->setPosition(pr->position);
		    rBlockOut->setNumOcurrences(lblockSize);
		    //m_currOutputPosition += blockSize;
		    m_currOutputPosition ++;
		    if (!rBlockIn->hasNext()) {
		      rBlockIn->resetBlock();
		      if (!(rBlockIn=blocks[++curblocki])) {
			rBlockIn = blocks[0];
			curblocki=0;
			lBlockIn=l_src->getNextValBlock(l_predcol);
		      }
		      else
			lBlockIn->resetBlock();
		    }
		    return;
		  }
		  }
		  rBlockIn->resetBlock();
		  rBlockIn=blocks[++curblocki];
		}
		rBlockIn = blocks[0];
		curblocki=0;
	      }
	      
	      if (lBlockIn == NULL) 
		lBlockOut = rBlockOut = NULL;
	  }
      }
     
  }
}

ValPos* NLJoin::getCurrLPair() {
  return currLPair;
}
ValPos* NLJoin::getNextLPair() {
  ValPos* p;
  if (!(p=lBlockIn->getNext())) {
    lBlockIn=l_src->getNextValBlock(l_predcol);
    if (lBlockIn) {
      currLPair = lBlockIn->getNext();
      return currLPair;
    }
    else
      return NULL;
  }
  else {
    currLPair=p;
    return p;
  }
}

ValPos* NLJoin::getNextLPairMC() {
  //Pair* p;
  //if (!lBlockIn->hasNext()) {
  lBlockIn=l_src->getNextValBlock(l_predcol);
  lBlockInExtra=l_src->getNextValBlock(1-l_predcol);
  if (lBlockIn) {
    currLPair = lBlockIn->getNext();
    return currLPair;
  }
  else
    return NULL;
  //}
//else {
// p=lBlockIn->getNext();
// currLPair=p;
//return p;
//}
}

PosBlock* NLJoin::getNextPosBlock(int colIndex_) {
  //if (!doEM)
  //  return getNextPosBlockMC(colIndex_);
  // check if we need to generate output new output blocks
  if (m_isLBlockConsumed && m_isRBlockConsumed) {
    m_isLBlockConsumed = false;
    m_isRBlockConsumed = false;
    generateBlocks();
  }

  if (colIndex_ == L_COL_INDEX && !m_isLBlockConsumed) {

    m_isLBlockConsumed = true;
    if (dontcheck)
      m_isRBlockConsumed = true;
    Log::writeToLog("NLJoin", 1, "Returning L Block");
    return ((PosBlock*) lBlockOut);

  } else if (colIndex_ == R_COL_INDEX && !m_isRBlockConsumed) {

    m_isRBlockConsumed = true;
    if (dontcheck)
      m_isLBlockConsumed = true;
    Log::writeToLog("NLJoin", 1, "Returning R Block");
    return ((PosBlock*)  rBlockOut);

  } else {
    // operator above is reading a block without having read other block
    if (!m_isLBlockConsumed) {
      throw new UnexpectedException("NLJoin: already read R Block but haven't read L Block");
    } else {
      throw new UnexpectedException("NLJoin: already read L Block but haven't read R Block");
    }
  }
  //return NULL;
}

//unsupported
PosBlock* NLJoin::getNextPosBlock(int colIndex_, int val_) {
  return NULL;
}

//Aggregator::AGG_TYPE Aggregator::getAggType() {
//  return m_aggType;
//}

void NLJoin::logOutputBlock(int count) {
  Log::writeToLog("NLJoin", 1, "Output Position: ", m_currOutputPosition);
  Log::writeToLog("NLJoin", 1, "  count = ", count);
}

void NLJoin::readSvalInnerTable() {
	sblocks = new int*[1500000]; // Yuck, but it's what generateBlocks does...
	sblocksizes = new int[1500000];
	SMultiBlock* r_blockIn = (SMultiBlock*) this->r_src->getNextSValBlock(0);
	if (!r_blockIn) {
	  sblocks[0]=NULL;
	  return;
	}
	int i = 0;
	rightFirstBlock= ((SMultiBlock*)r_blockIn->clone(*r_blockIn));
	sblocks[i] = (int*)rightFirstBlock->getBuffer();
	sblocksizes[i] = rightFirstBlock->getNElements();
	i++;
	// Meaningless integer argument to getNextSValBlock
	while( (r_blockIn = (SMultiBlock*) this->r_src->getNextSValBlock(0)) != NULL) {
		sblocks[i] = (int*)((SMultiBlock*)r_blockIn->clone(*r_blockIn))->getBuffer();
		sblocksizes[i] = r_blockIn->getNElements();
		i++;
	}
	sblocks[i] = NULL; // Make sure the first invalid pointer is NULL
	//cerr << "First invalid block is at: " << i << endl;
}
