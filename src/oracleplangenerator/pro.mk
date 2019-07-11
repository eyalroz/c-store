# This makefile builds the sample programs in $(ORACLE_HOME)/precomp/demo/proc
# and can serve as a template for linking customer applications.

# include $(ORACLE_HOME)/precomp/lib/env_precomp.mk

include ./env_precomp.mk

# SAMPLES is a list of the c proc sample programs.
# CPPSAMPLES is a list of the c++ proc sample programs.
# OBJECT_SAMPLES is a list of the proc sample programs using the new
# type features.  You must install ott in addition to proc to run some
# of those samples.
SAMPLES=sample1 sample2 sample3 sample4 sample6 sample7 sample8 \
	sample9 sample10 sample11 sample12 oraca sqlvcp cv_demo \
	ansidyn1 ansidyn2 cpdemo1 cpdemo2 scdemo1 scdemo2
CPPSAMPLES=cppdemo1 cppdemo3
OBJECT_SAMPLES=objdemo1 lobdemo1 coldemo1 navdemo1

# These targets build all of a class of samples in one call to make.
samples: $(SAMPLES)
cppsamples: $(CPPSAMPLES)
object_samples: $(OBJECT_SAMPLES) 

# The target 'build' puts together an executable $(EXE) from the .o files
# in $(OBJS) and the libraries in $(PROLDLIBS).  It is used to build the
# c sample programs.
# The rules to make .o files from .c and .pc files are later in this file.
# $(PROLDLIBS) uses the client shared library; $(STATICPROLDLIBS) does not.
#
build: $(OBJS)
	$(DEMO_PROC_BUILD_SHARED)
build_static: $(OBJS)
	$(DEMO_PROC_BUILD_STATIC)

$(SAMPLES) $(OBJECT_SAMPLES):
	$(MAKE) -f $(MAKEFILE) OBJS=$@.o EXE=$@ build

# sample5.pc illustrates the precompiler part of building a precompiler-forms
# application.  Building the actual executable would require the FORMS bundle.
sample5:
	$(SILENT)$(ECHO) 'sample5 is a user-exit demo; use a forms makefile to build it.'

# The c++ samples are built using the 'cppbuild' target.  It precompiles to
# get a .c file, compiles to get a .o file and then builds the executable.
#
cppbuild: 
	$(PROC) $(PROCPPFLAGS) iname=$(EXE)
	$(C++2O_EXE)
	$(DEMO_PROC_CPPBUILD_SHARED)

cppbuild_static: 
	$(PROC) $(PROCPPFLAGS) iname=$(EXE)
	$(C++2O_EXE)
	$(DEMO_PROC_CPPBUILD_STATIC)

$(CPPSAMPLES): cppdemo2
	$(MAKE) -f $(MAKEFILE) OBJS=$@.o EXE=$@ cppbuild

cppdemo2: empclass.o
	$(MAKE) -f $(MAKEFILE) OBJS="cppdemo2.o empclass.o" EXE=$@ cppbuild
cppdemo2_static: empclass.o
	$(MAKE) -f $(MAKEFILE) OBJS="cppdemo2.o empclass.o" EXE=cppdemo2 cppbuild_static

# Some of the samples require that .sql scripts be run before precompilation.
# If you set RUNSQL=run in your call to make, then make will use sqlplus or
# svrmgrl, as appropriate, to run the .sql scripts.
# If you leave RUNSQL unset, then make will print out a reminder to run the
# scripts.
# If you have already run the scripts, then RUNSQL=done will omit the reminder.
sqlplus_run:
	($(CD) ../sql; $(BINHOME)sqlplus $(USER) @$(SCRIPT) < /dev/null)
svrmgrl_run:
	($(CD) ../sql; $(BINHOME)svrmgrl < $(SCRIPT).sql)
sqlplus_ svrmgrl_:
	$(SILENT)$(ECHO) "# You must run the .sql script, " $(SCRIPT),
	$(SILENT)$(ECHO) "# before precomping this sample."
sqlplus_done svrmgrl_done:
#
calldemo-sql:
	$(MAKE) -f $(MAKEFILE) USER=$(USERID) SCRIPT=calldemo sqlplus_$(RUNSQL)
sample11-sql:
	$(MAKE) -f $(MAKEFILE) USER=$(USERID) SCRIPT=sample11 sqlplus_$(RUNSQL)
cv_demo-sql:
	$(MAKE) -f $(MAKEFILE) USER=$(USERID) SCRIPT=cv_demo sqlplus_$(RUNSQL)
cppdemo2-sql:
	$(MAKE) -f $(MAKEFILE) USER=$(USERID) SCRIPT=cppdemo2 sqlplus_$(RUNSQL)
lobdemo1-sql:
	$(MAKE) -f $(MAKEFILE) USER=$(USERID) SCRIPT=lobdemo1c sqlplus_$(RUNSQL)
objdemo1-sql:
	$(MAKE) -f $(MAKEFILE) USER=$(USERID) SCRIPT=objdemo1 sqlplus_$(RUNSQL)
coldemo1-sql:
	$(MAKE) -f $(MAKEFILE) USER=$(USERID) SCRIPT=coldemo1 sqlplus_$(RUNSQL)
navdemo1-sql:
	$(MAKE) -f $(MAKEFILE) USER=$(USERID) SCRIPT=navdemo1 sqlplus_$(RUNSQL)

# Here are some rules for converting .pc -> .c -> .o and for .typ -> .h.
#
# If proc needs to find .h files, it should find the same .h files that the 
# c compiler finds.  We use a macro named INCLUDE to hadle that.  The general 
# format of the INCLUDE macro is 
#   INCLUDE= $(I_SYM)dir1 $(I_SYM)dir2 ...
#
# Normally, I_SYM=-I, for the c compiler.  However, we have a special target,
# pc1, which calls $(PROC) with various arguments, including $(INCLUDE).  It
# is used like this:
#	$(MAKE) -f $(MAKEFILE) <more args to make> I_SYM=include= pc1
# This is used for some of $(SAMPLES) and for $(OBJECT_SAMPLES).
.SUFFIXES: .pc .c .o .typ .h

pc1:
	$(PCC2C)

.pc.c:
	$(MAKE) -f $(MAKEFILE) PROCFLAGS="$(PROCFLAGS)" PCCSRC=$* I_SYM=include= pc1

.pc.o:
	$(MAKE) -f $(MAKEFILE) PROCFLAGS="$(PROCFLAGS)" PCCSRC=$* I_SYM=include= pc1
	$(C2O)

.c.o:
	$(C2O)

.typ.h:
	$(OTT) intype=$*.typ hfile=$*.h outtype=$*o.typ $(OTTFLAGS) code=c userid=$(USERID)

# These are specific targets to make the .o files for samples that require
# more careful handling.
sample9.o: sample9.pc calldemo-sql
	$(PROC) $(PROCPLSFLAGS) iname=$*
	$(C2O)

cv_demo.o: cv_demo.pc cv_demo-sql
	$(PROC) $(PROCPLSFLAGS) iname=$*
	$(C2O)

sample11.o: sample11.pc sample11-sql
	$(PROC) $(PROCPLSFLAGS) iname=$*
	$(C2O)

sample4.o: sample4.pc
	$(MAKE) -f $(MAKEFILE) PCCSRC=$* I_SYM=include= pc1
	$(C2O)

ansidyn1.o: ansidyn1.pc
	$(PROC) mode=ansi iname=$*
	$(C2O)

ansidyn2.o: ansidyn2.pc
	$(PROC) dynamic=ansi iname=$*
	$(C2O)
	
cpdemo2.o: cpdemo2.pc
	$(PROC) cpool=yes cmin=5 cmax=14 cincr=2 iname=$*
	$(C2O)

cpdemo1.o: cpdemo1.pc
	$(PROC) cpool=yes iname=$*
	$(C2O)

empclass.o: empclass.pc cppdemo2-sql
	$(MAKE) -f $(MAKEFILE) PROCFLAGS="$(PROCPLSFLAGS) $(PROCPPFLAGS)" PCCSRC=$* I_SYM=include= pc1
	$(C++2O)

lobdemo1.o: lobdemo1.pc lobdemo1-sql
	$(MAKE) -f $(MAKEFILE) PROCFLAGS="$(PROCPLSFLAGS)" PCCSRC=$* I_SYM=include= pc1
	$(C2O)

objdemo1.o: objdemo1.pc objdemo1.typ objdemo1-sql
	$(OTT) intype=$*.typ hfile=$*.h outtype=$*o.typ $(OTTFLAGS) code=c userid=$(USERID)
	$(MAKE) -f $(MAKEFILE) PROCFLAGS=intype=$*o.typ PCCSRC=$* I_SYM=include= pc1
	$(C2O)

coldemo1.o: coldemo1.pc coldemo1.typ coldemo1-sql
	$(OTT) intype=$*.typ hfile=$*.h outtype=$*o.typ $(OTTFLAGS) code=c userid=$(USERID)
	$(MAKE) -f $(MAKEFILE) PROCFLAGS=intype=$*o.typ PCCSRC=$* I_SYM=include= pc1
	$(C2O)

navdemo1.o: navdemo1.pc navdemo1.typ navdemo1-sql
	$(OTT) intype=$*.typ hfile=$*.h outtype=$*o.typ $(OTTFLAGS) code=c userid=$(USERID)
	$(MAKE) -f $(MAKEFILE) PROCFLAGS=intype=$*o.typ PCCSRC=$* I_SYM=include= pc1
	$(C2O)

# These macro definitions fill in some details or override some defaults
MAKEFILE=$(ORACLE_HOME)/precomp/demo/proc/demo_proc.mk
OTTFLAGS=$(PCCFLAGS)
PROCPLSFLAGS=sqlcheck=full userid=$(USERID)
PROCPPFLAGS=code=cpp $(CPLUS_SYS_INCLUDE)
USERID=tpchscale10/scale10
INCLUDE=$(I_SYM). $(I_SYM)$(PRECOMPHOME)public $(I_SYM)$(RDBMSHOME)public $(I_SYM)$(RDBMSHOME)demo $(I_SYM)$(PLSQLHOME)public $(I_SYM)$(NETWORKHOME)public
