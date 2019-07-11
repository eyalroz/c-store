CPP_SRCS += \
${addprefix $(ROOT)/parser/BExpressions/BComparison/, \
BEq.cpp \
BGe.cpp \
BGt.cpp \
BLe.cpp \
BLt.cpp \
BNe.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
parser/BExpressions/BComparison/%.o: $(ROOT)/parser/BExpressions/BComparison/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '

# @g++ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $< && \
#g++ -MM -MG -P -w -O0 -g3 -Wall -c -fmessage-length=0  $< >> $(@:%.o=%.d)
