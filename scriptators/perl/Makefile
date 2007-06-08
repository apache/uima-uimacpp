ifeq ($(UIMACPP_HOME),)
  $(error UIMACPP_HOME not set)
endif

SWIGVERSION:=$(subst SWIG Version ,,$(filter SWIG Version 1.3.%,$(shell swig -version 2>&1)))

ifeq ($(SWIGVERSION),)
  $(error unable to determine SWIGVERSION)
endif

SWIGDEPS=uimaperlwrap.h
SWIGFLAGS=-DSWIG$(subst .,_,$(SWIGVERSION))

###################################
# This portion was divined by looking at the base.mak file

# name of the annotator to be created
TARGET_FILE=perltator

# list of user's object files to be linked when building the annotator
OBJS=Perltator.o uima_wrap.o

#Use this var to pass additional user-defined parameters to the compiler
USER_CFLAGS=-fPIC -g `perl -MExtUtils::Embed -e ccopts` $(SWIGFLAGS) -I..

#Use this var to pass additional user-defined parameters to the linker
USER_LINKFLAGS=-g `perl -MExtUtils::Embed -e ldopts` 

# Define the symbol DEBUG=1 if you want to build a debug version
DEBUG=1

# build a shared library
DLL_BUILD=1

# include file with generic compiler instructions
include $(UIMACPP_HOME)/lib/base.mak
###################################

test:
	echo "$(SWIGVERSION)"
	echo done

perltator.pm uima_wrap.cxx: ../uima.i
	swig -o uima_wrap.cxx -outdir . -c++ -perl ../uima.i

uimaperlwrap.h:
	swig -outdir . -c++ -perl -external-runtime $@

Perltator.o: $(SWIGDEPS) ../ThreadAnnotator.h

uima_wrap.o: uima_wrap.cxx
	$(CC) $(CFLAGS) -c $<

distclean: clean
	rm -f uima_wrap.cxx perltator.pm uimaperlwrap.h perltator.so
