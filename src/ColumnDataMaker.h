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
#ifndef COLUMNDATAMAKER_H
#define COLUMNDATAMAKER_H

#include <ctime>
#include <iostream>

#include "AM/AM.h"
#include "AM/PageWriter.h"
#include "AM/PagePlacer.h"
#include "AM/WOSAM.h"
#include "AM/WOSManager.h"
#include "common/Constants.h"
#include "common/Util.h"
#include "Operators/ColumnExtracter.h"
#include "Paths.h"
#include "Wrappers/Encoder/RLEEncoderII.h"
#include "Wrappers/CodingException.h"
#include "Wrappers/Decoder/RLEDecoderII.h"
#include "Wrappers/RLEIIDataSource.h"
#include "Writers/RLEWriter.h"

class ColumnDataMaker {
public:
    ColumnDataMaker();
    virtual ~ColumnDataMaker();
    void processInputArg(int argc, char **argv);
    void makeColumn();
    
    private:
    int total_parameter;
    std::string input_data_filename;
    int input_data_column_index;
    bool input_data_sorted;
    std::string output_data_filename;
    bool output_split_on_value;
    int force_rebuild_flag;
    int column_data_type;
    int compression_type;
    int column_index;
    bool storage_key_column;
    int buffer_size_in_bits;
    int value_size;
    int start_position;
    int repetition_size;
    int number_indices;
    bool primary_index_on_position;
    std::string wos_projection_name;
    int wos_number_indices;
    int	wos_number_columns;
};

#endif // COLUMNDATAMAKER_H
