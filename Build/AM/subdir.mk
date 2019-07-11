ifndef _CSTOREESSENCE_
CPP_SRCS += \
${addprefix $(ROOT)/AM/, \
WOSAM.cpp \
NOBDBAM.cpp \
WSMV.cpp \
ROSWOSSegment.cpp \
CatalogInstance.cpp \
WOSManager.cpp \
DVAM.cpp \
ROSManager.cpp \
}
endif

CPP_SRCS += \
${addprefix $(ROOT)/AM/, \
AccessException.cpp \
AM.cpp \
ROSAM.cpp \
PagePlacer.cpp \
PageWriter.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
AM/%.o: $(ROOT)/AM/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '

#@g++ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $< && \
#g++ -MM -MG -P -w -O0 -g3 -Wall -c -fmessage-length=0  $< >> $(@:%.o=%.d)
