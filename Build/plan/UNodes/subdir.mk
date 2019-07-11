CPP_SRCS += \
${addprefix $(ROOT)/plan/UNodes/, \
UAggAvgNode.cpp \
UAggCountNode.cpp \
UAggMaxNode.cpp \
UAggMinNode.cpp \
UAggNode.cpp \
UAggSumNode.cpp \
UCopyNode.cpp \
UNegationNode.cpp \
UProjectNode.cpp \
UInsertColumnNode.cpp \
UDeleteProjectionNode.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
plan/UNodes/%.o: $(ROOT)/plan/UNodes/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '






