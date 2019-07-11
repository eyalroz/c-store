CPP_SRCS += \
${addprefix $(ROOT)/plan/MNodes/, \
MConcatNode.cpp \
MResultNode.cpp \
MDeleteProjectionNode.cpp \
MInsertProjectionNode.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
plan/MNodes/%.o: $(ROOT)/plan/MNodes/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '






