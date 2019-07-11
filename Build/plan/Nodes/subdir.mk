CPP_SRCS += \
${addprefix $(ROOT)/plan/Nodes/, \
SelectNode.cpp \
CommitNode.cpp \
InsertNode.cpp \
DeleteProjectionNode.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Nodes/%.o: $(ROOT)/plan/Nodes/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '






