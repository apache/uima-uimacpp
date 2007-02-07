#!/bin/bash

usage() {
	echo "  Usage: buildsdk targetDirectory"
	echo "    Builds SDK for distribution."
	echo     buildsdk must be run from the root of the uimacpp source tree.
	echo "    The uimacpp project must be built and installed by running"
	echo "      'make install' in the directory of the uimacpp source tree."
	echo "    The doxygen documentation must be built by running buildocs.sh"
	echo "      in the docs directory of the uimacpp source tree."
	echo "    Requires the following environment variables:"
	echo "      UIMA_INSTALL - 'install' location of uimacpp build."
	echo "                     Defaults to ./install"
	echo "      APR_HOME - root of the APR install."
	echo "      ICU_HOME - root of the ICU install."
	echo "      XERCES_HOME - root of the XERCES install."
}


copylicenses() {
	echo copying licenses...

	# add copy of Apache SDK licenses at some point

	mkdir "$UIMA_DIR"/licenses/apr
	if [ -r "$APR_HOME"/LICENSE ]; then
  	  cp -p "$APR_HOME"/LICENSE* "$UIMA_DIR"/licenses/apr/
	else
	  echo "WARNING: LICENSE file for APR must be copied to $UIMA_DIR"/licenses/apr
	fi
	mkdir "$UIMA_DIR"/licenses/icu
	cp -p "$ICU_HOME"/license* "$UIMA_DIR"/licenses/icu/
	mkdir "$UIMA_DIR"/licenses/xerces
	if [ -r "$XERCES_HOME"/LICENSE ]; then
  	  cp -p "$XERCES_HOME"/LICENSE* "$UIMA_DIR"/licenses/xerces/
	else
	  echo "WARNING: LICENSE file for XERCES must be copied to $UIMA_DIR"/licenses/xerces
	fi
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
	export UIMA_INSTALLDIR=$UIMACPP_SOURCE/install
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


mkdir "$UIMA_DIR"
mkdir "$UIMA_DIR"/bin
mkdir "$UIMA_DIR"/data
mkdir "$UIMA_DIR"/docs
mkdir "$UIMA_DIR"/include
mkdir "$UIMA_DIR"/include/uima
mkdir "$UIMA_DIR"/lib
mkdir "$UIMA_DIR"/licenses
mkdir "$UIMA_DIR"/examples

UNAME=`uname -s`
if [ "$UNAME" = "Darwin" ]; then
  CPL=-p
  CPLR=-pR
  LIBEXT=dylib
else
  CPL=-pl
  CPLR=-plr
  LIBEXT=so
fi

echo copying from "$UIMA_INSTALLDIR"...
cp -p  "$UIMA_INSTALLDIR/bin/runAECpp" "$UIMA_DIR"/bin/

cp $CPLR "$UIMA_INSTALLDIR"/data  "$UIMA_DIR"/
cp -p "$UIMA_INSTALLDIR"/include/uima/* "$UIMA_DIR"/include/uima/
eval cp $CPLR "$UIMA_INSTALLDIR"/lib/libuima*.$LIBEXT"*"  "$UIMA_DIR"/lib/
if [ "$UNAME" = "Darwin" ]; then
  thisDir=`pwd`
  cd "$UIMA_DIR"/lib && ln -s libuima-2.1.dylib  libuima.jnilib
  cd $thisDir
fi
	
echo copying from "$UIMACPP_SOURCE/docs"...
cp -p "$UIMACPP_SOURCE"/docs/QuickStart.html "$UIMA_DIR"/docs/
cp -p "$UIMACPP_SOURCE"/docs/uimadoxytags.tag  "$UIMA_DIR"/docs/
cp $CPLR "$UIMACPP_SOURCE"/docs/html "$UIMA_DIR"/docs/

cp $CPLR "$UIMACPP_SOURCE"/examples/* "$UIMA_DIR"/examples/

# copy following file from fvt suite
cp $CPL "$UIMACPP_SOURCE"/src/test/src/SofaStreamHandlerFile.cpp "$UIMA_DIR"/examples/src/
cp $CPL "$UIMACPP_SOURCE"/src/test/src/SimpleTextSegmenter.cpp "$UIMA_DIR"/examples/src/

cp $CPL "$UIMACPP_SOURCE/src/base.mak" "$UIMA_DIR"/lib/
	
echo copying from "$APR_HOME"...
cp $CPLR  "$APR_HOME"/include/apr-* "$UIMA_DIR"/include/
eval cp $CPLR "$APR_HOME"/lib/libapr*.$LIBEXT"*" "$UIMA_DIR"/lib/

echo copying from "$ICU_HOME"...
cp $CPLR "$ICU_HOME"/include/unicode "$UIMA_DIR"/include/
eval cp $CPLR "$ICU_HOME"/lib/libicuio*.$LIBEXT"*" "$UIMA_DIR"/lib/
eval cp $CPLR "$ICU_HOME"/lib/libicuuc*.$LIBEXT"*" "$UIMA_DIR"/lib/
eval cp $CPLR "$ICU_HOME"/lib/libicui18n*.$LIBEXT"*" "$UIMA_DIR"/lib/
eval cp $CPLR "$ICU_HOME"/lib/libicudata.$LIBEXT"*" "$UIMA_DIR"/lib/
	
echo copying from "$XERCES_HOME"...
cp $CPLR "$XERCES_HOME"/include/xercesc "$UIMA_DIR"/include/
eval cp $CPLR "$XERCES_HOME"/lib/libxerces-c*.$LIBEXT"*" "$UIMA_DIR"/lib/

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
