#ifndef _CSTOREESSENCE_
#CPP_SRCS += \
#${addprefix $(ROOT)/, \
#TestDataSrc.cpp \
#}
#endif
#
CPP_SRCS += \
${addprefix $(ROOT)/, \
main.cpp \
QueryAny.cpp \
}


# Each subdirectory must supply rules for building sources it contributes
%.o: $(ROOT)/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '




