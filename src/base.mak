#############################################################################
# Makefile for UIMACPP components on UNIX
# using gcc/g++

 # Licensed to the Apache Software Foundation (ASF) under one
 # or more contributor license agreements.  See the NOTICE file
 # distributed with this work for additional information
 # regarding copyright ownership.  The ASF licenses this file
 # to you under the Apache License, Version 2.0 (the
 # "License"); you may not use this file except in compliance
 # with the License.  You may obtain a copy of the License at
 # 
 #   http://www.apache.org/licenses/LICENSE-2.0
 # 
 # Unless required by applicable law or agreed to in writing,
 # software distributed under the License is distributed on an
 # "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 # KIND, either express or implied.  See the License for the
 # specific language governing permissions and limitations
 # under the License.

#
# This file is included from specific makefiles to build
#   UIMA C++ annotators and applications
#
# Change below to customize your platform
#   For example, the extension for shared libraries specified by DLL_SUFFIX
#
# Individual components can add custom compile and link controls
#   such as USER_CFLAGS
#
# Variables which must have been previously set:
#
# UIMACPP_HOME           for the base of the UIMA C++ tree
#
# TARGET_FILE=foo   to create foo.$(DLL_SUFFIX) (shared lib)
#                   or foo (application)
# DEBUG=1           to build for debug
# DLL_BUILD=1       to build a shared library
# OBJS              to specify the user's object files to include in the link
#
# Parent makefile optionally may set:
#
# USER_CLFAGS       to specify additional compiler parameters
# USER_LINKFLAGS    to specify additional linker parameters
#
#############################################################################

ifeq ($(UIMACPP_HOME),)
  $(error UIMACPP_HOME not set)
endif

ifeq ($(TARGET_FILE),)
  $(error TARGET_FILE not set)
endif

# the suffix for shared libraries
DLL_SUFFIX=so
UNAME = $(shell uname -s)
ifeq ($(UNAME), Darwin)
  DLL_SUFFIX=dylib
endif

ifeq ($(DEBUG),1)
# compile flags for debug mode
BUILD_CFLAGS=-DDEBUG -g -fno-inline -fno-default-inline
BUILD_LFLAGS=
else
# compile flags for ship mode:
# all optimization on, Multithreaded, dynamic link runtime
BUILD_CFLAGS=-DNDEBUG -DTRACEOFF -O3
BUILD_LFLAGS= -Wl,--strip-debug
endif 

# include directory for compile
INCLUDES=-I$(UIMACPP_HOME)/include -I$(UIMACPP_HOME)/include/apr-1 -I$(UIMACPP_HOME)/include/apr-0

# compiler flags:
CFLAGS=-Wall -x c++ $(BUILD_CFLAGS) $(INCLUDES) $(USER_CFLAGS) \
	 -Wno-deprecated

ifeq ($(DLL_BUILD),1)
DLL_LINKFLAGS=-shared
TARGET_FILE_NAME=$(TARGET_FILE).$(DLL_SUFFIX)
BIN_LINKFLAGS=
else
BIN_LINKFLAGS= -lxerces-c -licuuc -licuio -licui18n -licudata -ldl
TARGET_FILE_NAME=$(TARGET_FILE)
endif

LINKFLAGS= $(DLL_LINKFLAGS) $(BIN_LINKFLAGS) \
	 $(USER_LINKFLAGS) $(BUILD_LFLAGS) \
	 -L$(UIMACPP_HOME)/lib -luima

# compiler to use
CC=g++

# rules to generate object files from c/c++ source files
.SUFFIXES : .o .c .cpp

.cpp.o :
	$(CC) $(CFLAGS) -c $<

.c.o :
	$(CC) $(CFLAGS) -c $<

all:  $(TARGET_FILE_NAME)

$(TARGET_FILE_NAME):  $(OBJS)
	$(CC)  $(OBJS) \
	$(LINKFLAGS) \
	-o $(TARGET_FILE_NAME)

clean:
	-rm -f $(OBJS)
	-rm -f $(TARGET_FILE_NAME)
