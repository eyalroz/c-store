CPP_SRCS += \
${addprefix $(ROOT)/plan/BNodes/, \
BAggAvgNode.cpp \
BAggCountNode.cpp \
BAggMaxNode.cpp \
BAggMinNode.cpp \
BAggNode.cpp \
BAggSumNode.cpp \
BConjunctionNode.cpp \
BDisjunctionNode.cpp \
BJoinNode.cpp \
BProjectNode.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
plan/BNodes/%.o: $(ROOT)/plan/BNodes/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '






