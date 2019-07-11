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
 *  modified version of QueryAny.cpp that uses performance API(PAPI) for collecting performance
 *  counter data (NOTE: PAPI is installed on Shamrock)
 *  to compiler PAPI-enabled QueryAny: make papi
 *  the executable is named cstoreqptestPAPI.
 */
#include "QueryAny.h"

QueryAny::QueryAny() {
}

QueryAny::~QueryAny() {
}

/* args isn't used here */
bool QueryAny::run(Globals* g, const vector<string>& args) {
  StopWatch stopWatch;
  stopWatch.start();
  bool success;

#ifdef CSTORE_PAPI
  long_long elapsed_us = 0;
  int EventSet=PAPI_NULL;
  const PAPI_hw_info_t *hwinfo = NULL;

  const unsigned int eventlist[] = {
	  PAPI_TOT_CYC,
	  PAPI_TOT_INS,
	  PAPI_TLB_TL,
	  PAPI_BR_MSP,
	  PAPI_L1_DCM,
	  PAPI_L1_ICM,
	  PAPI_L2_TCM,
          0x40270001,
#if 0	  
	  //+ counters returned values on Shamrock
	  PAPI_TOT_CYC,
	  PAPI_TOT_INS,
	  PAPI_RES_STL,
	  PAPI_L1_DCM,
	  PAPI_L1_ICM,
	  PAPI_L2_TCM,
	  PAPI_TLB_DM,
	  PAPI_BR_TKN,
	  PAPI_L1_ICA,
	  PAPI_TOT_IIS,
	  //-
	  PAPI_BR_CN,
	  PAPI_BR_INS,
	  PAPI_BR_MSP,
	  PAPI_BR_NTK,
	  PAPI_BR_PRC,
	  PAPI_BR_TKN,
	  PAPI_BR_UCN,
	  PAPI_BRU_IDL,
	  PAPI_BTAC_M,
	  PAPI_CA_CLN,
	  PAPI_CA_INV,
	  PAPI_CA_ITV,
	  PAPI_CA_SHR,
	  PAPI_CA_SNP,
	  PAPI_CSR_FAL,
	  PAPI_CSR_SUC,
	  PAPI_CSR_TOT,
	  PAPI_FAD_INS,
	  PAPI_FDV_INS,
	  PAPI_FMA_INS,
	  PAPI_FML_INS,
	  PAPI_FNV_INS,
	  PAPI_FP_INS,
	  PAPI_FP_OPS,
	  PAPI_FP_STAL,
	  PAPI_FPU_IDL,
	  PAPI_FSQ_INS,
	  PAPI_FUL_CCY,
	  PAPI_FUL_ICY,
	  PAPI_FXU_IDL,
	  PAPI_HW_INT,
	  PAPI_INT_INS,
	  PAPI_L1_DCA,
	  PAPI_L1_DCH,
	  PAPI_L1_DCM,
	  PAPI_L1_DCR,
	  PAPI_L1_DCW,
	  PAPI_L1_ICA,
	  PAPI_L1_ICH,
	  PAPI_L1_ICM,
	  PAPI_L1_ICR,
	  PAPI_L1_ICW,
	  PAPI_L1_LDM,
	  PAPI_L1_STM,
	  PAPI_L1_TCA,
	  PAPI_L1_TCH,
	  PAPI_L1_TCM,
	  PAPI_L1_TCR,
	  PAPI_L1_TCW,
	  PAPI_L2_DCA,
	  PAPI_L2_DCH,
	  PAPI_L2_DCM,
	  PAPI_L2_DCR,
	  PAPI_L2_DCW,
	  PAPI_L2_ICA,
	  PAPI_L2_ICH,
	  PAPI_L2_ICM,
	  PAPI_L2_ICR,
	  PAPI_L2_ICW,
	  PAPI_L2_LDM,
	  PAPI_L2_STM,
	  PAPI_L2_TCA,
	  PAPI_L2_TCH,
	  PAPI_L2_TCM,
	  PAPI_L2_TCR,
	  PAPI_L2_TCW,
	  PAPI_L3_DCA,
	  PAPI_L3_DCH,
	  PAPI_L3_DCM,
	  PAPI_L3_DCR,
	  PAPI_L3_DCW,
	  PAPI_L3_ICA,
	  PAPI_L3_ICH,
	  PAPI_L3_ICM,
	  PAPI_L3_ICR,
	  PAPI_L3_ICW,
	  PAPI_L3_LDM,
	  PAPI_L3_STM,
	  PAPI_L3_TCA,
	  PAPI_L3_TCH,
	  PAPI_L3_TCM,
	  PAPI_L3_TCR,
	  PAPI_L3_TCW,
	  PAPI_LD_INS,
	  PAPI_LST_INS,
	  PAPI_LSU_IDL,
	  PAPI_MEM_RCY,
	  PAPI_MEM_SCY,
	  PAPI_MEM_WCY,
	  PAPI_PRF_DM,
	  PAPI_RES_STL,
	  PAPI_SR_INS,
	  PAPI_STL_CCY,
	  PAPI_STL_ICY,
	  PAPI_SYC_INS,
	  PAPI_TLB_DM,
	  PAPI_TLB_IM,
	  PAPI_TLB_SD,
	  PAPI_TLB_TL,
	  PAPI_TOT_CYC,
	  PAPI_TOT_IIS,
	  PAPI_TOT_INS,
	  //#if 0
#endif
    0
   };

  map<unsigned int, long_long> eventInfo;
     
  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
	  cout << "*** PAPI_library_init failed! *** error:" << retval << endl;
	  exit (1);
  }
  if ((hwinfo = PAPI_get_hardware_info()) == NULL) {
    cout << "*** PAPI_get_hardware_info failed! *** error:" << retval << endl;
    exit (1);
  }

  if ((retval = PAPI_create_eventset(&EventSet)) != PAPI_OK) {
    cout << "*** PAPI_create_eventset failed! *** error:" << retval << endl;
    exit (1);
  }
  
  printCacheInfo(hwinfo);
  
  cout << "\t ===> setting up PAPI events" << endl;
  setupPAPIEvent(EventSet, eventlist, eventInfo);
  printPAPIEventInfo(EventSet, eventInfo);

#endif

#ifdef CSTORE_PAPI
  // query_plan();
  CatalogInstance* pCat = CatalogInstance::getCatalog(); //this will load WOS
  
  elapsed_us = PAPI_get_real_usec();  
	if ((retval = PAPI_reset(EventSet)) != PAPI_OK) {
		cout << "*** PAPI_reset failed ***" << endl;
      exit(1);
	}
#endif

	//query_plan contains QueryPlan.ccode
	query_plan();
  
#ifdef CSTORE_PAPI

  readPAPIEventInfo(EventSet, eventInfo);

  elapsed_us = PAPI_get_real_usec() - elapsed_us;
  
  printPAPIEventInfo(EventSet, eventInfo);
  
  cout << "\n\t ===> PAPI time counter: " << elapsed_us << " us\n" << endl;
 
  if (pCat) delete pCat;
   
#endif
  
  char* temp3 = new char[30];
  sprintf(temp3, "QueryAny.dat");
  //ofstream flStream;
  //flStream.open(temp3, ios::out | ios::app);
  //flStream.seekp(ios::end);
  stopWatch.stopToFile(temp3);
  //flStream.close();
  return success;
 
}

/**
 * Extract and report the cache information
 */
#ifdef CSTORE_PAPI
void QueryAny::printCacheInfo(const PAPI_hw_info_t *hwinfo) 
{
    int i, j;
	 PAPI_mh_level_t *L;	 
    L = (PAPI_mh_level_t *) &(hwinfo->mem_hierarchy.level[0]);
    for (i=0; i<hwinfo->mem_hierarchy.levels; i++) {
        for (j=0; j<2; j++) {
            int tmp;
            tmp = PAPI_MH_CACHE_TYPE(L[i].cache[j].type);
            if (tmp == PAPI_MH_TYPE_UNIFIED)
            { printf("L%d Unified ",i+1);} 
            else if (tmp == PAPI_MH_TYPE_DATA)
            {         printf("L%d Data ",i+1);}
            else if (tmp == PAPI_MH_TYPE_INST)
            { printf("L%d Instruction ",i+1); } 
            else if (tmp == PAPI_MH_TYPE_EMPTY)
            { break; }
            else
            { cout << "*** PAPI_get_hardware_info failed ***" << endl;  }
        
            tmp = PAPI_MH_CACHE_WRITE_POLICY(L[i].cache[j].type);
            if (tmp == PAPI_MH_TYPE_WB)
            { printf("Write back ");} 
            else if (tmp == PAPI_MH_TYPE_WT)
            { printf("Write through ");} 
            else 
            { cout << "*** PAPI_get_hardware_info failed ***" << endl;  }
        
            tmp = PAPI_MH_CACHE_REPLACEMENT_POLICY(L[i].cache[j].type);
            if (tmp == PAPI_MH_TYPE_PSEUDO_LRU)
            { printf("Pseudo LRU policy "); } 
            else if (tmp == PAPI_MH_TYPE_LRU)
            { printf("LRU policy ");} 
            else if (tmp == PAPI_MH_TYPE_UNKNOWN)
            { printf("Unknown policy "); }
            else
            { cout << "*** PAPI_get_hardware_info failed ***" << endl;  }
        
            printf("Cache:\n");
            if (L[i].cache[j].type) {
              printf("  Total size: %dKB\n  Line size: %dB\n  Number of Lines: %d\n  Associativity: %d\n\n",
                    (L[i].cache[j].size)>>10, L[i].cache[j].line_size, L[i].cache[j].num_lines, L[i].cache[j].associativity);
            }
        }
    }
}

void QueryAny::setupPAPIEvent(int EventSet, const unsigned int* eventlist,
	map<unsigned int, long_long>&eventInfo)
{
	int retval = 0, numCounters = 0;
	char descr[PAPI_MAX_STR_LEN];

	if ((numCounters = PAPI_get_opt(PAPI_MAX_HWCTRS,NULL)) > 0) {
		cout << "\t total " << numCounters << " counter(s) available!" << endl;
	}
	
	for (int i = 0; eventlist[i] != 0; i++) 
	{
		if (PAPI_query_event(eventlist[i]) == PAPI_OK) {
			eventInfo[eventlist[i]] = 0;
		}
	}
	map<unsigned int, long_long>::iterator iter;
	for (iter = eventInfo.begin(); iter != eventInfo.end(); ++iter) {
		PAPI_event_code_to_name((*iter).first, descr);
		PAPI_add_event(EventSet, (*iter).first);
		PAPI_event_code_to_name((*iter).first, descr);
		cout << "added " << "event " << descr << " " << (*iter).first << endl;
	}
	if ((retval = PAPI_start(EventSet)) != PAPI_OK) {
      cout << "*** PAPI_start failed ***" << endl;
      exit(1);
	}
	if ((retval = PAPI_reset(EventSet)) != PAPI_OK) {
		cout << "*** PAPI_reset failed ***" << endl;
      exit(1);
	}
}
  
void QueryAny::printPAPIEventInfo(int EventSet, map<unsigned int, long_long>&eventInfo) 
{
	char descr[PAPI_MAX_STR_LEN];
	PAPI_event_info_t evinfo;

	cout << endl;
	map<unsigned int, long_long>::iterator iter;
	for (iter = eventInfo.begin(); iter != eventInfo.end(); ++iter) {
		PAPI_event_code_to_name((*iter).first, descr);
		if (PAPI_get_event_info((*iter).first, &evinfo) != PAPI_OK) {
			cout << "*** PAPI_get_event_info failed! *** error!" << endl;
			exit(1);
		}
		cout.setf(ios::left,ios::adjustfield);
		cout.width(15);
		cout << evinfo.symbol << ' ';
		cout.setf(ios::left,ios::adjustfield);
		cout.width(60);
		cout << evinfo.long_descr << ' ';
		cout.setf(ios::right,ios::adjustfield);
		cout.width(20);
		cout << (*iter).second << endl;
  }
}

void QueryAny::readPAPIEventInfo(int EventSet, map<unsigned int, long_long>&eventInfo)
{
	if (eventInfo.size() > 0) {
		int retval = 0;
		long_long* values = new long_long[eventInfo.size()];
		if (retval = PAPI_read(EventSet, values) != PAPI_OK) {
			cout << "*** PAPI_read failed! *** error:" << retval << endl;
			exit(1);
		}
		for (unsigned int k=0; k <eventInfo.size(); ++k) {
			cout << "=>" << values[k] << endl;
		}
		map<unsigned int, long_long>::iterator iter;
		int i = 0;
		for (iter = eventInfo.begin(); iter != eventInfo.end(); ++iter) {
			(*iter).second = values[i];
			++i;
		}
		delete [] values;
	}	
}

#endif

void QueryAny::query_plan()
{
#include "QueryPlan.ccode"
}
