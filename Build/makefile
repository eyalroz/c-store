ROOT := ../src

-include makefile.init

RM := rm -rf

default: debug

# All of the sources participating in the build are defined here
-include sources.mk
-include $(SUBDIRS:%=%/subdir.mk)
-include objects.mk
-include $(DEPS)

debug: cstoreqp TAGS

cstoreqp: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Libs are:  $(LIBS)'
	$(CC) -o $@ $(OBJS) $(LFLAGS) $(LIBS) $(OPTS) 
	@echo 'Finished building: $@'

clean:
	-$(RM) $(OBJS) $(DEPS) $(INTERMEDIATES) cstoreqp
	-$(RM) *.o *.d HDRS TAGS

.PHONY: all clean dependents

-include $(ROOT)/makefile.targets
