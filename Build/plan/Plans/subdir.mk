CPP_SRCS += \
${addprefix $(ROOT)/plan/Plans/, \
SelectPlan.cpp \
InsertPlan.cpp \
DeletePlan.cpp \
CommitPlan.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
plan/Plans/%.o: $(ROOT)/plan/Plans/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '






