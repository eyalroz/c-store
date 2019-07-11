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
// Decodes Bit Pages
// Problems: migfer@mit.edu


#include "StopWatch.h"
#include <fstream>
#include <iostream>

StopWatch::StopWatch()
{
	totalReal=0;
}

StopWatch::~StopWatch()
{
  //using namespace std;
	//cout << "Total Lapped time: " << totalReal << endl;
}

void StopWatch::start() {

       r1 = times(&t);
}

double StopWatch::stop() {
  //clock_t diff = clock()-watch;
           r2 = times(&u);
           printf("user time=%f\n",
                 ((float)(u.tms_utime-t.tms_utime))/(HZ));
           printf("system time=%f\n",
                 ((float)(u.tms_stime-t.tms_stime))/(HZ));
           printf("real time=%f\n",
                 ((float)(r2-r1))/(HZ));
	   //return (((double) diff)/((double) CLOCKS_PER_SEC))*1000;
	   return (((float)(r2-r1))/(HZ))*1000;
}

double StopWatch::stopToFile(const char* x) {
  ofstream flStream;
  flStream.open(x, ios::out | ios::app);
  flStream.seekp(ios::end);
  //clock_t diff = clock()-watch;
  r2 = times(&u);
  flStream << "user time=" <<
    ((float)(u.tms_utime-t.tms_utime))/(HZ) << std::endl;
  flStream << "system time=" <<
    ((float)(u.tms_stime-t.tms_stime))/(HZ) << std::endl;
  flStream << "real time=" <<
    ((float)(r2-r1))/(HZ) << std::endl;
  //return (((double) diff)/((double) CLOCKS_PER_SEC))*1000;
  return (((float)(r2-r1))/(HZ))*1000;
}

double StopWatch::secondsPerTick()
{
  // constant factor, measured on sorrel. Should be measured at run time
  // by sleeping for some time.
  return (1/0.820578)*((1<<21) / 3.9e9);
}

double StopWatch::quietStop()
{
           r2 = times(&u);
	   // user time!
	   return  ((float)(u.tms_utime-t.tms_utime))/(HZ);
//(((float)(r2-r1))/(HZ))*1000;
}

void StopWatch::lapStart() {
	start();
}
void StopWatch::lapStop() {
	totalReal+=stop();
}
