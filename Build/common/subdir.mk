CPP_SRCS += \
${addprefix $(ROOT)/common/, \
Block.cpp \
BlockForceContig.cpp \
BlockPosNonContig.cpp \
BlockWithPos.cpp \
DataSource.cpp \
DatasourceFilters.cpp \
DataSourcePosFilter.cpp \
Exception.cpp \
PosBlock.cpp \
PosBlockArray.cpp \
PosPair.cpp \
Predicate.cpp \
UnexpectedException.cpp \
UnimplementedException.cpp \
Writer.cpp \
MinicolBlock.cpp \
ValPos.cpp \
IntValPos.cpp \
NullValPos.cpp \
LongValPos.cpp \
FloatValPos.cpp \
DoubleValPos.cpp \
StringValPos.cpp \
SPair.cpp \
SBlock.cpp \
}
#Pair.cpp \


# Each subdirectory must supply rules for building sources it contributes
common/%.o: $(ROOT)/common/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


# DSM 
