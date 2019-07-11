ifndef _CSTOREESSENCE_
CPP_SRCS += \
${addprefix $(ROOT)/Operators/, \
IntConverter.cpp \
InsertOperator.cpp \
DeleteOperator.cpp \
DVConverter.cpp \
BAndOperator.cpp \
BOrOperator.cpp \
BNotOperator.cpp \
Dict2Int.cpp \
Int2Dict.cpp \
Ident.cpp \
ShortCutDict2RLE.cpp \
ValBlockToPosBlockConverter.cpp \
MinicolConcat.cpp\
EMTuplePrinter.cpp \
BlockWithPosToMinicolConverter.cpp\
MinicolShimOperator.cpp\
Merge.cpp \
MergeSortedGroups.cpp \
Union.cpp \
Dict2RLE.cpp \
Type2BlockToMinicolConverter.cpp\
Concat.cpp \
Filter.cpp \
TestDataSrc.cpp \
}
endif
#RLEBlockToMinicolConverter.cpp\
#MinicolMaterializer.cpp\

CPP_SRCS += \
${addprefix $(ROOT)/Operators/, \
BlockPrinter.cpp \
AbstractAggregator.cpp \
PipelinedAggregator.cpp \
HashAggregator.cpp \
Aggregator.cpp \
BCopyOperator.cpp \
Average.cpp \
ColumnExtracter.cpp \
SColumnExtracter.cpp \
Count.cpp \
Min.cpp \
Max.cpp \
NLJoin.cpp \
HashJoin.cpp \
Operator.cpp \
Select.cpp \
Sum.cpp \
SlowCount.cpp \
PosAndOperator.cpp \
PosOrOperator.cpp \
SBlockPrinter.cpp \
TupleGenerator.cpp\
EMHashAggregator.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Operators/%.o: $(ROOT)/Operators/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '

# DSM




