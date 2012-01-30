#!/bin/ksh

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

# for now, change versionnumber in line below
srcName=uimacpp-2.4.0-src

if [ "$1" = 'TRACE' ]
    then set -vx ;shift
fi

if [ $# -lt 1 ] ;then
    print -u2 "\nERROR: target directory for source tree not specified"
    print -u2 "-------"
    print -u2 "Usage: buildSrcTree targetDirectory [clean]"
    print -u2 "       Builds Unix source for distribution"
    print -u2 "assumes: current directory is the root of svn extract"
    print -u2 "expects: target directory to be non-existant\n"
    print -u2 "-------"
    exit 9
fi

target_dir=$1/$srcName
if [ "$2" = 'clean' ]
    then rm -rf $target_dir
fi
if [ -d $target_dir ]
then
    print -u2 "ERROR: directory $target_dir already exists"
    print -u2 "use "clean" option"
    print -u2 "FAILED: UIMA C++ source tree was not built."
    exit 9
fi
print -u2 "Target Source directory is $target_dir"

UIMACPP_SOURCE=`pwd`
if [ ! -d "$UIMACPP_SOURCE"/src ]; then
       echo ERROR: current directory is not root of UIMACPP source tree
       exit
fi

UNAME=`uname -s`
if [ "$UNAME" = "Darwin" ]; then
  CP=-p
  CPL=-p
  CPLR=-pR
  LIBEXT=dylib
else
  CP=-pH
  CPL=-pd
  CPLR=-pdr
  LIBEXT=so
fi


# No Configure script means we have to run autogen
if [ -x "./configure" ] ; then
        # even if we have a configure script, if the template has been updated we should
        # run it again to account for the changes, which also means we need to run a autogen
        # to create the actual script and associated autoconf artifacts.
    if [ "./configure.ac" -ot "./configure" ]  ; then
             echo "configure is up to date"
    else
            ./autogen.sh
    fi
else
  ./autogen.sh
fi


mkdir $target_dir
mkdir $target_dir/m4
mkdir $target_dir/data
mkdir $target_dir/docs
mkdir $target_dir/src
mkdir $target_dir/src/cas
mkdir $target_dir/src/cas/uima
mkdir $target_dir/src/jni
mkdir $target_dir/src/jni/uima
mkdir $target_dir/src/framework
mkdir $target_dir/src/framework/uima
mkdir $target_dir/src/utils
mkdir $target_dir/src/test

print -u2 "copying from $UIMACPP_SOURCE"
cp $CPL  $UIMACPP_SOURCE/buildsdk.sh $target_dir/
cp $CPL  $UIMACPP_SOURCE/NOTICE $target_dir/NOTICE
cp $CPL  $UIMACPP_SOURCE/LICENSE $target_dir/
cp $CPL  $UIMACPP_SOURCE/RELEASE_NOTES* $target_dir/
cp $CPL  $UIMACPP_SOURCE/README.4src $target_dir/README
cp $CPL  $UIMACPP_SOURCE/README.4bin $target_dir/
cp $CPL  $UIMACPP_SOURCE/NOTICE.4bin $target_dir/
cp $CPL  $UIMACPP_SOURCE/configure $target_dir/
cp $CPLR $UIMACPP_SOURCE/config $target_dir
cp $CPL  $UIMACPP_SOURCE/Makefile.in $target_dir
cp $CPL  $UIMACPP_SOURCE/configure.ac $target_dir
cp $CPL  $UIMACPP_SOURCE/autogen.sh $target_dir
cp $CPL  $UIMACPP_SOURCE/Makefile.am $target_dir
cp $CPL  $UIMACPP_SOURCE/m4/find_icu.m4  $target_dir/m4
cp $CPL  $UIMACPP_SOURCE/m4/find_xerces.m4 $target_dir/m4
cp $CPL  $UIMACPP_SOURCE/m4/find_apr.m4 $target_dir/m4
cp $CPL  $UIMACPP_SOURCE/m4/find_activemq.m4 $target_dir/m4
cp $CPL  $UIMACPP_SOURCE/aclocal.m4  $target_dir

print -u2 "copying from $UIMACPP_SOURCE/data"
cp $CPL  $UIMACPP_SOURCE/data/*.xsd $target_dir/data/

print -u2 "copying from $UIMACPP_SOURCE/docs"
cp $CPL $UIMACPP_SOURCE/docs/uimacppdocs.mak $target_dir/docs
cp $CPL $UIMACPP_SOURCE/docs/builddocs.sh $target_dir/docs
cp $CPL $UIMACPP_SOURCE/docs/uimacpp.dox $target_dir/docs
cp $CPL $UIMACPP_SOURCE/docs/doxyheader.html $target_dir/docs
cp $CPL $UIMACPP_SOURCE/docs/overview_and_setup.html $target_dir/docs

print -u2 "copying from $UIMACPP_SOURCE/src"
cp $CPL $UIMACPP_SOURCE/src/base.mak $target_dir/src/
cp $CPL $UIMACPP_SOURCE/src/Makefile.in $target_dir/src/
cp $CPL $UIMACPP_SOURCE/src/Makefile.am $target_dir/src/

print -u2 "copying from $UIMACPP_SOURCE/src/cas"
cp $CPL $UIMACPP_SOURCE/src/cas/*.cpp $target_dir/src/cas/
cp $CPL $UIMACPP_SOURCE/src/cas/uima/*.hpp $target_dir/src/cas/uima/
cp $CPL $UIMACPP_SOURCE/src/cas/uima/*.inl $target_dir/src/cas/uima/

print -u2 "copying from $UIMACPP_SOURCE/src/framework"
cp $CPL $UIMACPP_SOURCE/src/framework/*.cpp $target_dir/src/framework/
cp $CPL $UIMACPP_SOURCE/src/framework/*.in $target_dir/src/framework/
cp $CPL $UIMACPP_SOURCE/src/framework/uima/*.hpp $target_dir/src/framework/uima/
cp $CPL $UIMACPP_SOURCE/src/framework/uima/*.h $target_dir/src/framework/uima/
cp $CPL $UIMACPP_SOURCE/src/framework/*.am $target_dir/src/framework/

print -u2 "copying from $UIMACPP_SOURCE/src/jni"
cp $CPL $UIMACPP_SOURCE/src/jni/*.cpp $target_dir/src/jni/
cp $CPL $UIMACPP_SOURCE/src/jni/uima/*.hpp $target_dir/src/jni/uima/
cp $CPL $UIMACPP_SOURCE/src/jni/uima/*.h $target_dir/src/jni/uima/

print -u2 "copying from $UIMACPP_SOURCE/src/utils"
cp $CPL $UIMACPP_SOURCE/src/utils/*.cpp $target_dir/src/utils/
cp $CPL $UIMACPP_SOURCE/src/utils/*.hpp $target_dir/src/utils/
cp $CPL $UIMACPP_SOURCE/src/utils/*.in $target_dir/src/utils/
cp $CPL $UIMACPP_SOURCE/src/utils/*.am $target_dir/src/utils/

print -u2 "copying from $UIMACPP_SOURCE/scriptators"
cp $CPLR $UIMACPP_SOURCE/scriptators $target_dir/

print -u2 "copying from $UIMACPP_SOURCE/examples"
cp $CPLR $UIMACPP_SOURCE/examples $target_dir/

print -u2 "copying from $UIMACPP_SOURCE/src/test"
cp $CPLR $UIMACPP_SOURCE/src/test/src $target_dir/src/test/
cp $CPLR $UIMACPP_SOURCE/src/test/data $target_dir/src/test/
cp $CPL $UIMACPP_SOURCE/src/test/Makefile.am $target_dir/src/test/
cp $CPL $UIMACPP_SOURCE/src/test/Makefile.in $target_dir/src/test/
cp $CPL $UIMACPP_SOURCE/src/test/fvt.sh $target_dir/src/test/
cp $CPL $UIMACPP_SOURCE/src/test/fvtTestfile $target_dir/src/test/
find $target_dir/src/test -type f -iname '*.l?' -exec rm -f {} \;
find $target_dir/src/test -type f -iname '*.o' -exec rm -f {} \;

print -u2 "copying files required for a Windows build"
cp $CPL $UIMACPP_SOURCE/buildsdk.cmd $target_dir
cp $CPL $UIMACPP_SOURCE/docs/builddocs.cmd $target_dir/docs
cp $CPL $UIMACPP_SOURCE/src/*.cmd $target_dir/src
cp $CPL $UIMACPP_SOURCE/examples/*.cmd $target_dir/examples
cp $CPL $UIMACPP_SOURCE/src/*.bat $target_dir/src
cp $CPL $UIMACPP_SOURCE/src/test/*.bat $target_dir/src/test
cp $CPL $UIMACPP_SOURCE/src/*.vcproj $target_dir/src
cp $CPL $UIMACPP_SOURCE/src/test/src/*.vcproj $target_dir/src/test/src
cp $CPL $UIMACPP_SOURCE/src/test/fvtTestfile.debug $target_dir/src/test
cp $CPL $UIMACPP_SOURCE/examples/src/*.vcproj $target_dir/examples/src
cp $CPL $UIMACPP_SOURCE/examples/tutorial/src/*.vcproj $target_dir/examples/tutorial\src
cp $CPL $UIMACPP_SOURCE/src/*.sln $target_dir/src
cp $CPL $UIMACPP_SOURCE/src/test/*.sln $target_dir/src/test
cp $CLP $UIMACPP_SOURCE/examples/src/*.sln $target_dir/examples/src


print -u2 "removing unused files from $UIMACPP_SOURCE"
find $target_dir -depth -type d -name .svn -exec rm -rf {} \;
#find $target_dir -type f -iname '*.sln' -exec rm -f {} \;
#find $target_dir -type f -iname '*.vcproj' -exec rm -f {} \;
#find $target_dir -type f -iname '*.cmd' -exec rm -f {} \;
#find $target_dir -type f -iname '*.bat' -exec rm -f {} \;
find $target_dir -type f -iname '*~' -exec rm -f {} \;

print -u2 "DONE ... Source image created in $target_dir"

exit 0
