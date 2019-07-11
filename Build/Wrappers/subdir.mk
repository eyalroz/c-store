ifndef _CSTOREESSENCE_
CPP_SRCS += \
${addprefix $(ROOT)/Wrappers/, \
DictDataSource.cpp \
DictCPUDataSource.cpp \
LZDataSource.cpp \
NSDataSource.cpp \
RLEIIDataSource.cpp \
RLEVecDataSource.cpp \
}
endif
##MultiBlockWithPos.cpp \
##MultiBlockWithPosForceContig.cpp \
#LongBasicBlock.cpp \

CPP_SRCS += \
${addprefix $(ROOT)/Wrappers/, \
BasicBlock.cpp \
MultiBlock.cpp \
PosMultiBlock.cpp \
BitBlock.cpp \
Type2Block.cpp \
PosType2Block.cpp \
DictByteBlock.cpp \
DictShortBlock.cpp \
CodingException.cpp \
DeltaPosBlock.cpp \
DeltaPosDataSource.cpp \
Type2DataSource.cpp \
Type2DecompDataSource.cpp \
IntDataSource.cpp \
PosBasicBlock.cpp \
PosDeltaBlock.cpp \
PosRLEBlock.cpp \
RLEBlock.cpp \
RLEDataSource.cpp \
PosArrayBlock.cpp\
SparseBlock.cpp \
SparseDataSource.cpp \
BitSparseDataSource.cpp \
RangeSparseDataSource.cpp \
SBasicBlock.cpp \
SMultiBlock.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Wrappers/%.o: $(ROOT)/Wrappers/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


