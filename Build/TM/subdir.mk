CPP_SRCS += \
${addprefix $(ROOT)/TM/, \
TMMerger.cpp \
WosTuple.cpp \
WosTupleIter.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
TM/%.o: $(ROOT)/TM/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '

