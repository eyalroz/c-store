#
# Makefile for ColumnDataMaker
#
CC      = g++
CFLAGS  = -O0 -g3 -Wall -c -Wno-deprecated -fmessage-length=0 -pg
SLEEPYCAT_DIRECTORY := /usr/local/BerkeleyDB.4.2
SLEEPYCAT_INCLUDE := $(SLEEPYCAT_DIRECTORY)/include/
SLEEPYCAT_LIB := $(SLEEPYCAT_DIRECTORY)/lib
IFLAGS  = -I$(SLEEPYCAT_INCLUDE)
OPTS    = -pg -Wl,--export-dynamic -Wl,--demangle
LIBS    = -ldb-4.2 -ldb_cxx-4.2 -lpthread -llzo

CSTORE_OBJECT = common/Block.o common/DataSource.o common/DatasourceFilters.o common/DataSourcePosFilter.o  common/Exception.o common/Pair.o common/PosBlock.o common/PosPair.o common/Predicate.o common/UnexpectedException.o  common/UnimplementedException.o common/Writer.o Writers/BasicBlockWriter.o Writers/DeltaPosValue.o  Writers/DeltaPosWriter.o Writers/DictWriter.o Writers/NullWriter.o Writers/PosRLEWriter.o Writers/PosDeltaWriter.o  Writers/RLEWriter.o Wrappers/Util/BitReader.o Wrappers/Util/BitWriter.o Wrappers/Util/ByteReader.o  Wrappers/Util/ByteWriter.o Wrappers/Util/IntReader.o Wrappers/Util/IntWriter.o Wrappers/Encoder/DeltaEncoder.o  Wrappers/Encoder/DeltaPosEncoder.o Wrappers/Encoder/DictEncoder.o Wrappers/Encoder/Encoder.o  Wrappers/Encoder/IntEncoder.o Wrappers/Encoder/LZEncoder.o Wrappers/Encoder/NSEncoder.o Wrappers/Encoder/RLEEncoder.o  Wrappers/Encoder/RLEEncoderII.o Wrappers/Decoder/BitDecoder.o Wrappers/Decoder/Decoder.o  Wrappers/Decoder/DeltaPosDecoder.o Wrappers/Decoder/IntDecoder.o Wrappers/Decoder/LZDecoder.o  Wrappers/Decoder/NSDecoder.o Wrappers/Decoder/DictDecoder.o Wrappers/Decoder/RLEDecoder.o  Wrappers/Decoder/RLEDecoderII.o Wrappers/BasicBlock.o Wrappers/LongBasicBlock.o Wrappers/MultiBlock.o  Wrappers/PosMultiBlock.o Wrappers/BitBlock.o Wrappers/CodingException.o Wrappers/DeltaPosBlock.o  Wrappers/DeltaPosDataSource.o Wrappers/DictDataSource.o Wrappers/IntDataSource.o Wrappers/LZDataSource.o  Wrappers/NSDataSource.o Wrappers/PosBasicBlock.o Wrappers/PosDeltaBlock.o Wrappers/PosRLEBlock.o Wrappers/RLEBlock.o  Wrappers/RLEDataSource.o Wrappers/RLEIIDataSource.o Wrappers/RLEVecDataSource.o Util/HashMap.o Util/Log.o  Util/SpillMap.o Util/StopWatch.o Util/StringUtil.o Util/BDBFile.o TM/TMMerger.o TM/WosTuple.o TM/WosTupleIter.o  Operators/AbstractAggregator.o Operators/PipelinedAggregator.o Operators/HashAggregator.o Operators/Aggregator.o  Operators/BCopyOperator.o Operators/BAndOperator.o Operators/BOrOperator.o Operators/BNotOperator.o Operators/Average.o Operators/ColumnExtracter.o Operators/IntConverter.o Operators/Count.o Operators/Min.o  Operators/Max.o Operators/NLJoin.o Operators/Operator.o Operators/Select.o Operators/Sum.o Operators/SlowCount.o Operators/Merge.o Operators/MergeSortedGroups.o Operators/Union.o Operators/InsertOperator.o Operators/DeleteOperator.o Operators/DVConverter.o AM/AccessException.o AM/AM.o AM/WOSAM.o AM/ROSAM.o AM/PagePlacer.o AM/PageWriter.o AM/WSMV.o AM/ROSWOSSegment.o AM/CatalogInstance.o AM/WOSManager.o AM/DVAM.o AM/ROSManager.o parser/TVariable.o parser/PObject.o

LINK_OBJECT = ${addprefix ../Build/, $(CSTORE_OBJECT)}

SOURCES = ColumnDataMaker.cpp ColumnDataMaker.h
OBJECTS = ColumnDataMaker.o

EXECUTABLE = ColumnDataMaker

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo 'linking target: $@'
	@echo 'link objects: $(LINK_OBJECT)'
	$(CC) -Wno-deprecated -o $@ $(OBJECTS) $(LINK_OBJECT) $(IFLAGS) $(LIBS) $(OPTS)
	@echo 'Finished building: $@'		

.cpp.o:
	@echo 'compiling target: $@'
	$(CC) $(CFLAGS) $(IFLAGS) $< -o $@

.PHONY: all

