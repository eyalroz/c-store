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
 * DJD Experiments:
 */
#include "DJD_QueryAny.h"

DJD_QueryAny::DJD_QueryAny()
{
}

DJD_QueryAny::~DJD_QueryAny()
{
}

/* args isn't used here */
bool DJD_QueryAny::run(Globals* g, const vector<string>& args) {
  system ("rm DJD_QueryAny.out");
  Log::writeToLog("DJD_QueryAny", 10, "DJD_QueryAny starting...");
  bool success = true;
  
  Log::writeToLog("DJD_QueryAny", 10, "Opening column shipdate and suppkey...");

#ifdef CSTORE_PAPI
  Misc* pMisc = new Misc();
  pMisc->init_PAPI();
  pMisc->setup_PAPI_event();
#endif

  StopWatch stopWatch;
  stopWatch.start();

#ifdef CSTORE_PAPI
  //+ starting PAPI
  pMisc->start_PAPI_event();  long_long start_usec = pMisc->get_usec();
  long_long start_cyc = pMisc->get_cycle();
  //-
#endif

/**
 *   experiment code start
 */

#include "DJD_QueryAny_ccode.cpp"

/**
 *   experiment code end
 */


cout << "DJD_QueryAny took: " << stopWatch.stop() << " ms" <<  endl;

#ifdef CSTORE_PAPI
        //+ output PAPI 
        pMisc->read_PAPI_event();
        pMisc->print_PAPI_event();
        long_long elapsed_usec = pMisc->get_usec() - start_usec;
        long_long elapsed_cyc = pMisc->get_cycle() - start_cyc;
        cout << "elapsed time in usec:" << elapsed_usec << endl;
        cout << "elapsed cycles:" << elapsed_cyc << endl;
        //
#endif

return success;

}
