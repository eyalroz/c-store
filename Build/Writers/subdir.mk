CPP_SRCS += \
${addprefix $(ROOT)/Writers/, \
BasicBlockWriter.cpp \
DeltaPosValue.cpp \
DeltaPosWriter.cpp \
Type2Writer.cpp \
DictWriter.cpp \
DictByteWriter.cpp \
DictMultiWriter.cpp \
NullWriter.cpp \
PosRLEWriter.cpp \
PosDeltaWriter.cpp \
RLEWriter.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Writers/%.o: $(ROOT)/Writers/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $<  
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '



