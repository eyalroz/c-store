CPP_SRCS += \
${addprefix $(ROOT)/materialexps/, \
BAndRLERLE.cpp \
MinicolConcatTest.cpp \
PosAndLM.cpp \
PosAndMC.cpp \
PosAndEM.cpp \
LMDataMaker.cpp \
T2Test.cpp \
posand-support.cpp\
JoinLM.cpp\
JoinEM.cpp\
JoinMC.cpp\
}

# Each subdirectory must supply rules for building sources it contributes
materialexps/%.o: $(ROOT)/materialexps/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '

