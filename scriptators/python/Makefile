ifeq ($(UIMACPP_HOME),)
  $(error UIMACPP_HOME not set)
endif

SWIGVERSION:=$(subst SWIG Version ,,$(filter SWIG Version 1.3.%,$(shell swig -version 2>&1)))

ifeq ($(SWIGVERSION),)
  $(error unable to determine SWIGVERSION)
endif
ifeq ($(SWIGVERSION),1.3.29)
SWIGDEPS=uimapywrap.h
SWIGFLAGS=-DSWIG$(subst .,_,$(SWIGVERSION))
else
SWIGFLAGS=-DSWIG_GLOBAL -DSWIG$(subst .,_,$(SWIGVERSION))
endif


PYTHONVERSION:=$(subst ., ,$(subst Python ,,$(shell python -V 2>&1)))
ifeq ($(PYTHONVERSION),)
  $(error PYTHONVERSION could not be determined)
endif
PYTHONDIR=python$(word 1,$(PYTHONVERSION)).$(word 2,$(PYTHONVERSION))
PYTHONLIB=-l$(PYTHONDIR)
PYTHONFLAGS=-DPYTHONLIBRARYNAME=\"lib$(PYTHONDIR).so\" -DPYTHON$(word 1,$(PYTHONVERSION))_$(word 2,$(PYTHONVERSION))

###################################
# This portion was divined by looking at the base.mak file

# name of the annotator to be created
TARGET_FILE=_pythonnator

# list of user's object files to be linked when building the annotator
OBJS=pythonnator.o uima_wrap.o

#Use this var to pass additional user-defined parameters to the compiler
USER_CFLAGS=-g -I/usr/include/$(PYTHONDIR) $(SWIGFLAGS) $(PYTHONFLAGS)

#Use this var to pass additional user-defined parameters to the linker
USER_LINKFLAGS=-g $(PYTHONLIB)

# Define the symbol DEBUG=1 if you want to build a debug version
DEBUG=1

# build a shared library
DLL_BUILD=1

# include file with generic compiler instructions
include $(UIMACPP_HOME)/lib/base.mak
###################################

pythonnator.py uima_wrap.cxx: ../uima.i
	swig -o uima_wrap.cxx -outdir . -c++ -python ../uima.i

uimapywrap.h:
	swig -outdir . -c++ -python -external-runtime $@

pythonnator.o: $(SWIGDEPS)

uima_wrap.o: uima_wrap.cxx 
	$(CC) $(CFLAGS) -c $<

distclean: clean
	rm -f uima_wrap.cxx _pythonnator.so pythonnator.py \
		pythonnator.pyc uimapywrap.h
