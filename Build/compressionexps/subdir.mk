ifndef _CSTOREESSENCE_
CPP_SRCS += \
${addprefix $(ROOT)/compressionexps/, \
Int2DataMaker.cpp \
LZ2DataMaker.cpp \
NS2DataMaker.cpp \
RLE2DataMaker.cpp \
Dict2DataMaker.cpp \
DictCPU2DataMaker.cpp \
JoinExpDataMaker.cpp \
SelExpDataMaker.cpp \
Int2DataRunner.cpp \
LZ2DataRunner.cpp \
NS2DataRunner.cpp \
RLE2DataRunner.cpp \
Dict2DataRunner.cpp \
DictCPU2DataRunner.cpp \
JoinExpDataRunner.cpp \
JoinExpRLEDataRunner.cpp \
JoinExpDictCPUDataRunner.cpp \
JoinExpType2DataRunner.cpp \
SelExpDataRunner.cpp \
SelExpRLEDataRunner.cpp \
SelExpType2DataRunner.cpp \
SelExpDictCPUDataRunner.cpp \
DeltaPosDataMaker.cpp \
DeltaPosDataRunner.cpp \
DictDataMaker.cpp \
DictDataRunner.cpp \
DictCPUDataMaker.cpp \
DictCPUDataRunner.cpp \
DictShortCutDataRunner.cpp \
LZDataMaker.cpp \
LZDataRunner.cpp \
NSDataMaker.cpp \
NSDataRunner.cpp \
}
endif

CPP_SRCS += \
${addprefix $(ROOT)/compressionexps/, \
RLEDataMaker.cpp \
RLEDataRunner.cpp \
Type2DataMaker.cpp \
Type2DataRunner.cpp \
IntDataMaker.cpp \
IntDataRunner.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
compressionexps/%.o: $(ROOT)/compressionexps/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '

