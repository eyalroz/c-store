CPP_SRCS += \
${addprefix $(ROOT)/Util/, \
HashMap.cpp \
Log.cpp \
SpillMap.cpp \
StopWatch.cpp \
StringUtil.cpp \
BDBFile.cpp \
Misc.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Util/%.o: $(ROOT)/Util/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '
