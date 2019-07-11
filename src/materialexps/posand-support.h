#ifndef POSANDSUPPORT_H_
#define POSANDSUPPORT_H_

#include "../AM/ROSAM.h"
#include "../common/DataSource.h"
#include "../Wrappers/IntDataSource.h"
#include "../Wrappers/RLEDataSource.h"
#include "../Wrappers/Type2DecompDataSource.h"
#include "../Wrappers/Type2DataSource.h"
#include "../Operators/SBlockPrinter.h"
#include "../Operators/Filter.h"
#include "../Operators/TupleGenerator.h"
#include "../Wrappers/Decoder/UncompressedDecoder.h"
#include <string>

using namespace std;

DataSource* getLineNumIntDS(bool second_copy);
DataSource* getLineNumRLEDS(bool second_copy);
DataSource* getLineNumT2DecompDS(bool second_copy);
DataSource* getLineNumT2PosDS(bool second_copy);
DataSource* getLineNumDS(string type, bool second_copy=false);
void printEndOfTest(string name, int testnum, int shipnum, int linenum, string line_num_type, bool aggregate, int nseen, double time);

#endif /*POSANDSUPPORT_H_*/
