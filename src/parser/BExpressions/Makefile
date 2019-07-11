SHELL = /bin/sh
CC = $(WTF) g++

# For every new .C file created, you must add a corresponding .o to OBJS.
OBJS = BComparison.o BBinaryLogic.o BNegation.o BBool.o BIsNull.o

# Executables
EXEC = cstore
 
SLEEPYCAT_DIRECTORY = /usr/local/BerkeleyDB.4.2
#SLEEPYCAT_DIRECTORY = /pro/borealis/software/bdb/
SLEEPYCAT_INCLUDE = $(SLEEPYCAT_DIRECTORY)/include/
SLEEPYCAT_LIB = $(SLEEPYCAT_DIRECTORY)/lib
CSTORE_INCLUDE = ../../../..
SUBDIRS = BComparison BBinaryLogic

# Directories to search when we #include <blah.H>. (/usr/include is
# searched implicitly.)h
IFLAGS = -I$(SLEEPYCAT_INCLUDE) -I$(CSTORE_INCLUDE) -Wno-deprecated

#libraries to search for
LFLAGS = -L$(SLEEPYCAT_LIB)

LIBS = -ldb-4.2 -ldb_cxx-4.2  -lpthread 


# on MacOSX make often uses tcsh instead of bash, which doesn't define
# the OSTYPE environment variable; if that's the case, define it
ifndef OSTYPE
	OSTYPE := $(shell uname)
endif
# however, uname returns "Darwin" instead of "darwin" or "darwin6.0"
# so let's get things into lower case
# this is a hack, but I don't know any other way to do this strictly
# within make (ie, not requiring some shell feature or other program)
OSTYPE := $(subst Darwin,darwin,$(OSTYPE))

# sometimes OSTYPE=darwin6.0, not "darwin": so use findstring
ifeq (darwin, $(findstring darwin,$(OSTYPE))) # If OS is Mac
	LIBS := $(LIBS) -bind_at_load
endif

CFLAGS = -g

OPTS =

# We always depend on EXEC.
all: $(OBJS)
	cp *.o ../../../Debug/src
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
	(cd $$i; make all); done

# To create $(EXEC), we invoke CC to link everything together.
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LFLAGS) $(LIBS) $(OPTS)

# All .C files depend on their headers
%.cpp:	%.H

# We compile .C files to their corresponding .o in OBJS.
%.o:%.cpp
	$(CC) $(CFLAGS) $(IFLAGS) $(LFLAGS) $(OPTS) -c $<

# makedepend checks file dependencies and makes sure that everything
# that needs to be compiled is compiled. It's not an entirely speedy
# process, but you should run it after adding header files and/or
# making significant changes to your code base.
depend:
	 makedepend  $(IFLAGS) *.C

clean:
	rm -f $(EXEC)
	rm -f ir.out
	rm -rf Templates.DB
	rm -rf scale1*
	rm -rf SunWS_cache
	rm -f  *.o core *~
	@for i in $(SUBDIRS); do \
	echo "make clean in $$i..."; \
	(cd $$i; make clean); done

link:
	ln -s /data/eoneil/mycstore/tpch/dbgen/scale10/dna/bdbstuff/scale1* .
# DO NOT DELETE
