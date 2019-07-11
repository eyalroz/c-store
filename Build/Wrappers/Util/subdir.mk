CPP_SRCS += \
${addprefix $(ROOT)/Wrappers/Util/, \
BitReader.cpp \
BitWriter.cpp \
ByteReader.cpp \
ByteWriter.cpp \
IntReader.cpp \
IntWriter.cpp \
StringReader.cpp \
StringWriter.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Wrappers/Util/%.o: $(ROOT)/Wrappers/Util/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '
