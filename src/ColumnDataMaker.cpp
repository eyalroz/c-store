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
#include "ColumnDataMaker.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Util/StringUtil.h"

ColumnDataMaker::ColumnDataMaker()
{
}

ColumnDataMaker::~ColumnDataMaker()
{
}

int main(int argc, char **argv)
{
  using namespace std;
  system("cat license.txt");
  
  string promptUser;
  ColumnDataMaker* cdm = new ColumnDataMaker();
  cdm->processInputArg(argc, argv);
  cout << "\nReady to make this column? (y/n) ";
  cin >> promptUser;
  cout << endl;
  if (promptUser.find("y") == string::npos) {
    cout << "Aborting ColumnDataMaker..." << endl;
    exit(0);	
  }
  cdm->makeColumn();
  delete cdm;
  return 0;
}

void ColumnDataMaker::processInputArg(int argc, char **argv)
{
  if (argc < 2) {
    cerr << "usage: ./ColumnDataMaker input_config_file" << endl;
    exit(0);
  }

  string configFile = argv[1];
  ifstream fin;
  fin.open(configFile.c_str(), ifstream::in);
  if( !fin.is_open() )
  {
    cerr << "input file: " << configFile << " NOT found !!!" << endl;
    exit(0);
  }
  
  char buffer[256];
  while ( !fin.eof() ) {
    fin.getline (buffer, 255);
    string inLine = buffer;
    if(inLine[0] != '#' && inLine.size()>1){
      string varName;
      string varValue;
      int pos = inLine.find("=");
      assert(pos > 0);
      varName.assign(inLine, 0, pos);
      varValue.assign(inLine, pos + 1, inLine.size());
      total_parameter = 0;
      //cout << varName << ":" <<varValue << endl;
      if (varName.find("INPUT_DATA_FILENAME") != string::npos) {
        ++total_parameter;			
        input_data_filename.assign(varValue, 0, varValue.size());
        cout << varName << "=" << input_data_filename << endl;
        ++total_parameter;
      } else if (varName.find("INPUT_DATA_COLUMN_INDEX") != string::npos) {
        input_data_column_index = atoi(varValue.c_str());
        cout << varName << "=" << input_data_column_index << endl;
        ++total_parameter;
      } else if (varName.find("INPUT_DATA_SORTED") != string::npos) {
        input_data_sorted = atoi(varValue.c_str());
        cout << varName << "=" << input_data_sorted << endl;
        ++total_parameter;
      } else if (varName.find("OUTPUT_DATA_FILENAME") != string::npos) {
        output_data_filename = varValue;
        cout << varName << "=" << output_data_filename << endl;
        ++total_parameter;
      } else if (varName.find("FORCE_REBUILD") != string::npos) {
        force_rebuild_flag = atoi(varValue.c_str());
        cout << varName << "=" << force_rebuild_flag << endl;
        ++total_parameter;			
      } else if (varName.find("COLUMN_DATA_TYPE") != string::npos) {
        column_data_type = atoi(varValue.c_str());
        cout << varName << "=" << column_data_type << endl;
        ++total_parameter;			
      } else if (varName.find("COMPRESSION_TYPE") != string::npos) {
        compression_type = atoi(varValue.c_str());
        cout << varName << "=" << compression_type << endl;
        ++total_parameter;
      } else if (varName.find("COLUMN_INDEX") != string::npos) {
        column_index = atoi(varValue.c_str());
        cout << varName << "=" << compression_type << endl;
        ++total_parameter;
      } else if (varName.find("STORAGE_KEY_COLUMN") != string::npos) {
        storage_key_column = atoi(varValue.c_str());
        cout << varName << "=" << storage_key_column << endl;
        ++total_parameter;
      } else if (varName.find("BUFFER_SIZE_IN_BITS") != string::npos) {
        buffer_size_in_bits = atoi(varValue.c_str());
        cout << varName << "=" << buffer_size_in_bits << endl;
        ++total_parameter;			
      } else if (varName.find("VALUE_SIZE") != string::npos) {
        value_size = atoi(varValue.c_str());
        cout << varName << "=" << value_size << endl;
        ++total_parameter;			
      } else if (varName.find("START_POSITION") != string::npos) {
        start_position = atoi(varValue.c_str());
        cout << varName << "=" << start_position << endl;
        ++total_parameter;			
      } else if (varName.find("REPETITION_SIZE") != string::npos) {
        repetition_size = atoi(varValue.c_str());
        cout << varName << "=" << repetition_size << endl;
        ++total_parameter;			
      } else if (varName.find("NUMBER_INDICES") != string::npos) {
        number_indices = atoi(varValue.c_str());
        cout << varName << "=" << number_indices << endl;
        ++total_parameter;			
      } else if (varName.find("PRIMARY_INDEX_ON_POSITION") != string::npos) {
        primary_index_on_position = true;
        cout << varName << "=" << primary_index_on_position << endl;
        ++total_parameter;			
      } else if (varName.find("WOS_PROJECTION_NAME") != string::npos) {
        ++total_parameter;			
        wos_projection_name = varValue;
        cout << varName << "=" << wos_projection_name << endl;
        ++total_parameter;			
      } else if (varName.find("WOS_NUMBER_INDICES") != string::npos) {
        wos_number_indices = atoi(varValue.c_str());
        cout << varName << "=" << wos_number_indices << endl;
        ++total_parameter;
      } else if (varName.find("WOS_NUMBER_COLUMNS") != string::npos) {
        wos_number_columns = atoi(varValue.c_str());				
        cout << varName << "=" << wos_number_columns << endl;
        ++total_parameter;
      }
    }
  }
  fin.close();
}

void ColumnDataMaker::makeColumn()
{
  cout << "Extracting column data from " << input_data_filename << " ..." << endl;
  ColumnExtracter* ce = new ColumnExtracter(
      input_data_filename,      // filename
      input_data_column_index,  // column index
      force_rebuild_flag);      // force rebuild flag

  if (compression_type == 0) {
      IntEncoder* encoder = new IntEncoder(
          (Operator*) ce,                 // data source
          column_index,                   // column index
          buffer_size_in_bits);           // buffer size in bits                
      IntDecoder* decoder = new IntDecoder(input_data_sorted);  // value sorted
      PagePlacer* pagePlacer = new PagePlacer(
          encoder,
          decoder, 
          number_indices,                    // num indexes
          primary_index_on_position);        // position primary      
      cout << "Writing column data to " << output_data_filename << " ..." << endl;
      pagePlacer->placeColumn(output_data_filename,   // output data file name
                              output_split_on_value,  // split on value
                              input_data_sorted);     // value sorted
    
      if (storage_key_column) {
        cout << "Calling WOSManager..." << endl;
        WOSManager* wosMgr = new WOSManager(
            wos_projection_name,    // table name
            wos_number_indices,     // num indices
            wos_number_columns);    // num columns        
        delete wosMgr;
      }
      
      delete pagePlacer;
      delete decoder;
      delete encoder;
  } else if (compression_type == 1) {
      RLEEncoder* encoder = new RLEEncoder(
        (Operator*) ce,                 // data source
        column_index,                   // column index
        buffer_size_in_bits,            // buffer size in bits
        (byte)column_data_type,         // value type
        (short)value_size,              // value size
        (unsigned int)start_position,   // start pos size
        (unsigned int)repetition_size); // reps size
    
      RLEDecoder* decoder = new RLEDecoder(input_data_sorted);  // value sorted
      PagePlacer* pagePlacer = new PagePlacer(
        encoder,
        decoder, 
        number_indices,                   // num indexes
        primary_index_on_position);       // position primary
    
      cout << "Writing column data to " << output_data_filename << " ..." << endl;
      pagePlacer->placeColumn(output_data_filename, // output data file name
                              output_split_on_value,  // split on value
                              input_data_sorted);     // value sorted
      
      if (storage_key_column) {
	cout << "Calling WOSManager..." << endl;
	WOSManager* wosMgr = new WOSManager(
            wos_projection_name,    // table name
            wos_number_indices,     // num indices
            wos_number_columns);    // num columns     
	delete wosMgr;
      }
      delete pagePlacer;
      delete decoder;
      delete encoder;
  }
  delete ce;

  cout << "ColumnDataMaker Done..." << endl;
}
