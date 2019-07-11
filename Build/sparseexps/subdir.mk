CPP_SRCS += \
${addprefix $(ROOT)/sparseexps/, \
SelExpDM.cpp \
SelExpDR.cpp \
StrSelExpDM.cpp \
StrSelExpDR.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
sparseexps/%.o: $(ROOT)/sparseexps/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '

