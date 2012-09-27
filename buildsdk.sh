#!/bin/bash

#   Licensed to the Apache Software Foundation (ASF) under one
#   or more contributor license agreements.  See the NOTICE file
#   distributed with this work for additional information
#   regarding copyright ownership.  The ASF licenses this file
#   to you under the Apache License, Version 2.0 (the
#   "License"); you may not use this file except in compliance
#   with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing,
#   software distributed under the License is distributed on an
#   #  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#   KIND, either express or implied.  See the License for the
#   specific language governing permissions and limitations
#   under the License.

usage() {
	echo "  Usage: buildsdk targetDirectory"
	echo "    Builds SDK for distribution."
	echo     buildsdk must be run from the root of the uimacpp source tree.
	echo "    The uimacpp project must be built and installed by running"
	echo "      'make install' in the directory of the uimacpp source tree."
	echo "    The doxygen documentation must be built by running buildocs.sh"
	echo "      in the docs directory of the uimacpp source tree."
	echo "    Requires the following environment variables:"
	echo "      UIMA_INSTALLDIR - 'install' location of uimacpp build."
	echo "                     Defaults to ./usr/local/uimacpp"
	echo "      APR_HOME - root of the APR install."
	echo "      ICU_HOME - root of the ICU install."
	echo "      XERCES_HOME - root of the XERCES install."
 	echo "      ACTIVEMQ_HOME - root of the XERCES install." 
        echo "      APU_HOME - root of the APR Util install Required if ACTIVEMQ_HOME is set."
}


copylicenses() {
	echo copying licenses...

	# add Apache SDK licenses
	cp $CPL  $UIMACPP_SOURCE/NOTICE.4bin $UIMA_DIR/NOTICE
	cp $CPL  $UIMACPP_SOURCE/LICENSE $UIMA_DIR/
	cp $CPL  $UIMACPP_SOURCE/RELEASE_NOTES* $UIMA_DIR/
	cp $CPL  $UIMACPP_SOURCE/README.4bin $UIMA_DIR/README

# 	mkdir "$UIMA_DIR"/licenses/apr
# 	if [ -r "$APR_HOME"/LICENSE ]; then
#   	  cp -p "$APR_HOME"/LICENSE* "$UIMA_DIR"/licenses/apr/
# 	else
# 	  echo "WARNING: LICENSE file for APR must be copied to $UIMA_DIR"/licenses/apr
# 	fi
# 	mkdir "$UIMA_DIR"/licenses/icu
# 	cp -p "$ICU_HOME"/license* "$UIMA_DIR"/licenses/icu/
# 	mkdir "$UIMA_DIR"/licenses/xerces
# 	if [ -r "$XERCES_HOME"/LICENSE ]; then
#   	  cp -p "$XERCES_HOME"/LICENSE* "$UIMA_DIR"/licenses/xerces/
# 	else
# 	  echo "WARNING: LICENSE file for XERCES must be copied to $UIMA_DIR"/licenses/xerces
# 	fi
        return
}



# check args
if [  $# -lt 1 ]; then
	echo ERROR: SDK target directory not specified
	usage
	exit
fi

TARGET_DIR="$1"
mkdir $TARGET_DIR
if [ ! -d "$TARGET_DIR" ]; then
       echo ERROR: Could not create $TARGET_DIR
       exit
fi

UIMA_DIR=$TARGET_DIR/uimacpp

if [ "$2" = "clean" ]; then
	echo removing "$UIMA_DIR"
	rm -rf "$UIMA_DIR"
fi

if [ -d "$UIMA_DIR" ]; then
       echo ERROR: directory "$UIMA_DIR" already exists. Please use "clean" option
       exit
fi

echo SDK directory tree will be built in $UIMA_DIR

UIMACPP_SOURCE=`pwd`
if [ ! -d "$UIMACPP_SOURCE"/src ]; then
       echo ERROR: current directory is not root of UIMACPP source tree
       exit
fi

# set default values if not set

if [ "$UIMA_INSTALLDIR" = "" ]; then
	echo ERROR: UIMA_INSTALLDIR is not set.
       	exit
fi

if [[ "$APR_HOME" = "" || "$ICU_HOME" = "" || "$XERCES_HOME" = "" ]]; then
    echo Error: APR_HOME and ICU_HOME and XERCES_HOME must all be specified
    echo UIMACPP SDK was not built.
    exit
fi

echo check that the required directories contain files
if [ ! -r "$UIMA_INSTALLDIR"/include/uima/api.hpp ]; then
  	echo ERROR: uima/api.hpp not found
	echo UIMA_INSTALLDIR "$UIMA_INSTALLDIR" is invalid.
	echo Build and install UIMA C++ first by running::
        echo make
        echo make install
	echo UIMACPP SDK was  not built.
	exit
fi

if [ ! -r "$UIMA_INSTALLDIR"/bin/runAECpp ]; then
	echo ERROR: runAECpp not found.
        echo UIMA_INSTALLDIR "$UIMA_INSTALLDIR" is invalid.
	echo Build and install UIMA C++ first by running:
        echo make
        echo make install
	echo UIMACPP SDK was  not built.
	exit
fi

if [ ! -r "$UIMACPP_SOURCE"/docs/html/index.html ]; then
       echo ERROR: The UIMA C++ doxygen docs are missing.
       echo Build the docs by running
       echo cd docs
       echo . builddocs.sh
       echo UIMACPP SDK was not built.
       exit
fi

if [ ! -d "$APR_HOME"/include ]; then
       echo ERROR: APR_HOME "$APR_HOME" is invalid.
       echo UIMACPP SDK was not built.
       exit
fi

if [ ! -d "$ICU_HOME"/include ]; then
  	echo ERROR: ICU_HOME "$ICU_HOME" is invalid.
        echo UIMACPP SDK was not built
        exit
fi

if [ ! -d "$XERCES_HOME"/include ]; then
       echo ERROR: XERCESHOME "$XERCES_HOME" is invalid.
       echo UIMACPP SDK was not built.
fi

if [  -r "$UIMA_INSTALLDIR"/bin/deployCppService ]; then
   echo "checking $ACTIVEMQ_HOME"
   if [ "$ACTIVEMQ_HOME" = "" ]; then
      echo ERROR: ACTIVEMQ_HOME must be set to location of ActiveMQ CPP install.
      echo UIMACPP SDK was not built.
      exit
   fi
   echo "checking $APU_HOME"
   if [ "$APU_HOME" = "" ]; then
      echo ERROR: APU_HOME must be set to location where APR-Util is installed.
      echo UIMACPP SDK was not built.
      exit
   fi


   if [ ! -d "$ACTIVEMQ_HOME"/lib ]; then
      echo ERROR: ACTIVEMQ_HOME "$ACTIVEMQ_HOME" is invalid.
      echo UIMACPP SDK was not built.
      exit
   fi
   echo "checking $APU_HOME"
   if [ ! -d "$APU_HOME"/lib ]; then
      echo ERROR: APU_HOME "$APU_HOME"  is invalid.
      echo UIMACPP SDK was not built.
      exit
   fi
fi


mkdir "$UIMA_DIR"
mkdir "$UIMA_DIR"/bin
mkdir "$UIMA_DIR"/data
mkdir "$UIMA_DIR"/docs
mkdir "$UIMA_DIR"/include
mkdir "$UIMA_DIR"/include/uima
mkdir "$UIMA_DIR"/lib
#mkdir "$UIMA_DIR"/licenses

UNAME=`uname -s`
if [ "$UNAME" = "Darwin" ]; then
  CPL=-p
  CPLR=-pR
  LIBEXT=dylib
else
  CPL=-pl
  CPLR=-pdr
  LIBEXT=so
fi

echo copying from "$UIMA_INSTALLDIR"...
cp -p  "$UIMA_INSTALLDIR/bin/runAECpp" "$UIMA_DIR"/bin/
if [ ! "$ACTIVEMQ_HOME" = "" ]; then
  cp -p  "$UIMA_INSTALLDIR/bin/deployCppService" "$UIMA_DIR"/bin/
fi

cp $CPLR "$UIMA_INSTALLDIR"/data  "$UIMA_DIR"/
cp -p "$UIMA_INSTALLDIR"/include/uima/* "$UIMA_DIR"/include/uima/
eval cp $CPLR "$UIMA_INSTALLDIR"/lib/libuima*.$LIBEXT"*"  "$UIMA_DIR"/lib/
if [ "$UNAME" = "Darwin" ]; then
  thisDir=`pwd`
  cd "$UIMA_DIR"/lib && ln -s libuima.dylib  libuima.jnilib
  cd $thisDir
fi
	
echo copying from "$UIMACPP_SOURCE/docs"...
cp -p "$UIMACPP_SOURCE"/docs/overview_and_setup.html "$UIMA_DIR"/docs/
cp -p "$UIMACPP_SOURCE"/docs/uimadoxytags.tag  "$UIMA_DIR"/docs/
cp $CPLR "$UIMACPP_SOURCE"/docs/html "$UIMA_DIR"/docs/

cp $CPLR "$UIMACPP_SOURCE"/examples "$UIMA_DIR"/

# copy following files from fvt suite
cp $CPL "$UIMACPP_SOURCE"/src/test/src/SofaStreamHandlerFile.cpp "$UIMA_DIR"/examples/src/
cp $CPL "$UIMACPP_SOURCE"/src/test/src/SimpleTextSegmenter.cpp "$UIMA_DIR"/examples/src/
cp $CPL "$UIMACPP_SOURCE"/src/test/src/MeetingAnnotator.cpp "$UIMA_DIR"/examples/tutorial/src/
cp -p "$UIMACPP_SOURCE"/src/test/.libs/libMeetingAnnotator.$LIBEXT "$UIMA_DIR"/examples/tutorial/src/MeetingAnnotator.$LIBEXT
find $UIMA_DIR/examples -type f -iname '*.sln' -exec rm -f {} \;
find $UIMA_DIR/examples -type f -iname '*.vcproj' -exec rm -f {} \;
find $UIMA_DIR/examples -type f -iname '*.cmd' -exec rm -f {} \;


cp $CPL "$UIMACPP_SOURCE/src/base.mak" "$UIMA_DIR"/lib/
	
echo copying APR from "$APR_HOME"...
cp $CPLR  "$APR_HOME"/include/apr-* "$UIMA_DIR"/include/
eval cp $CPLR "$APR_HOME"/lib/libapr*.$LIBEXT"*" "$UIMA_DIR"/lib/

echo copying ICU from "$ICU_HOME"...
cp $CPLR "$ICU_HOME"/include/unicode "$UIMA_DIR"/include/
eval cp $CPLR "$ICU_HOME"/lib/libicuio*.$LIBEXT"*" "$UIMA_DIR"/lib/
eval cp $CPLR "$ICU_HOME"/lib/libicuuc*.$LIBEXT"*" "$UIMA_DIR"/lib/
eval cp $CPLR "$ICU_HOME"/lib/libicui18n*.$LIBEXT"*" "$UIMA_DIR"/lib/
eval cp $CPLR "$ICU_HOME"/lib/libicudata.$LIBEXT"*" "$UIMA_DIR"/lib/
	
echo copying Xerces from "$XERCES_HOME"...
cp $CPLR "$XERCES_HOME"/include/xercesc "$UIMA_DIR"/include/
eval cp $CPLR "$XERCES_HOME"/lib/libxerces-c*.$LIBEXT"*" "$UIMA_DIR"/lib/

if [ ! "$ACTIVEMQ_HOME" = "" ]; then
  echo copying ActiveMQ from "$ACTIVEMQ_HOME"...
  eval cp $CPLR "$ACTIVEMQ_HOME"/lib/libactivemq-cpp*.$LIBEXT"*" "$UIMA_DIR"/lib/
  echo copying APR-Util from "$APU_HOME"...
  eval cp $CPLR "$APU_HOME"/lib/libaprutil*.$LIBEXT"*" "$UIMA_DIR"/lib/
fi

echo copying the scriptators...
SCRIPTATORS_HOME=$UIMACPP_SOURCE/scriptators
cp $CPLR "$SCRIPTATORS_HOME" "$UIMA_DIR"/
cp -p "$SCRIPTATORS_HOME"/perl/Perl.html "$UIMA_DIR"/docs/
cp -p "$SCRIPTATORS_HOME"/python/Python.html "$UIMA_DIR"/docs/
cp -p "$SCRIPTATORS_HOME"/tcl/Tcl.html "$UIMA_DIR"/docs/

copylicenses

find "$UIMA_DIR" -name .svn | xargs rm -rf {}
find "$UIMA_DIR" -name '*~' | xargs rm -f {}

echo DONE. UIMACPP SDK image is in "$UIMA_DIR".
