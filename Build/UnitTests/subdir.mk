ifndef _CSTOREESSENCE_
CPP_SRCS += \
${addprefix $(ROOT)/UnitTests/, \
OptTestOneChoice.cpp \
OptTest.cpp \
OptPlanTest.cpp \
PlanData.cpp \
PlanDataFullScale.cpp \
QueryTest.cpp \
BDBReadWrite.cpp \
BitReaderWriter.cpp \
DeltaPosExtractLoadAndAccess.cpp \
DeltaPosWriterTest.cpp \
DictExtractLoad.cpp \
EMTest.cpp \
HashMapTest.cpp \
IntEncodeDecodeTest.cpp \
IntExtractAndLoad.cpp \
IntReaderWriterTest.cpp \
LoadTPCHData.cpp \
LZED.cpp \
LZEncodeDecode.cpp \
LZvsINT.cpp \
MergeTest.cpp \
JoinTest.cpp \
MultiLoadAndAccess.cpp \
MultipleColumnsTest.cpp \
NSEncodeDecode.cpp \
Queries.cpp \
Query100.cpp \
Query1.cpp \
Query2.cpp \
Query3.cpp \
Query4.cpp \
Query4Prime.cpp \
Query5.cpp \
Query6.cpp \
Query7.cpp \
Query1U.cpp \
Query2U.cpp \
Query3U.cpp \
Query4U.cpp \
Query5U.cpp \
Query6U.cpp \
Query7U.cpp \
Query1WSRS.cpp \
Query2WSRS.cpp \
Query3WSRS.cpp \
RLEBlockTest.cpp \
RLEEncodeDecode.cpp \
RLEEncodeDecodeII.cpp \
RLEExtractLoadAndAccess.cpp \
RLEWriterTest.cpp \
SpillMapTest.cpp \
SValJoinTest.cpp \
TmMergerTest.cpp \
Type2WriterTest.cpp \
Type2ExtractLoadAndAccess.cpp \
PosBlockTest.cpp \
BitProfileTest.cpp \
WOSTest.cpp \
WOSTest2.cpp \
DummyTest.cpp \
CatalogTest.cpp \
BCopyOpTest.cpp \
BAndOpTest.cpp \
BAndQueryTest.cpp \
BOrOpTest.cpp \
BNotOpTest.cpp \
BNotQueryTest.cpp \
BDBPerformanceProfile.cpp \
InsertOpTest.cpp \
DeleteOpTest.cpp \
DVTest.cpp \
SelectDV.cpp \
DJD_Data.cpp \
DJD_QueryAny.cpp \
PlanInC.cpp \
Plans.cpp \
GenerateBDBQueryData.cpp \
}
endif

CPP_SRCS += \
${addprefix $(ROOT)/UnitTests/, \
UnitTest.cpp \
ProjMaker.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
UnitTests/%.o: $(ROOT)/UnitTests/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


