CPP_SRCS += \
${addprefix $(ROOT)/plan/, \
Plan.cpp \
Node.cpp \
UNode.cpp \
BNode.cpp \
MNode.cpp \
BaseNode.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
plan/%.o: $(ROOT)/plan/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '






