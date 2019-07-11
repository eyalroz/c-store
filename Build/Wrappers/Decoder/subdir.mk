CPP_SRCS += \
${addprefix $(ROOT)/Wrappers/Decoder/, \
BitDecoder.cpp \
Decoder.cpp \
DeltaPosDecoder.cpp \
Type2Decoder.cpp \
IntDecoder.cpp \
StringDecoder.cpp \
UncompressedDecoder.cpp \
LZDecoder.cpp \
NSDecoder.cpp \
DictDecoder.cpp \
DictByteDecoder.cpp \
DictMultiDecoder.cpp \
DictDelayedDecoder.cpp \
ShortCutDictDelayedDecoder.cpp \
RLEDecoder.cpp \
RLEDecoderII.cpp \
RLEGenericDecoder.cpp \
SparseDecoder.cpp \
BitSparseDecoder.cpp \
RangeSparseDecoder.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Wrappers/Decoder/%.o: $(ROOT)/Wrappers/Decoder/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '
