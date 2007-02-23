#!/bin/ksh

# for now, change versionnumber in line below
srcName=uimacpp-2.1.0

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
  CPL=-pl
  CPLR=-plr
  LIBEXT=so
fi

mkdir $target_dir
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

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE"
cp $CPL  $UIMACPP_SOURCE/buildsdk.sh $target_dir/

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/data"
cp $CPL  $UIMACPP_SOURCE/data/*.xsd $target_dir/data/

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/docs"
cp $CPL $UIMACPP_SOURCE/docs/uimacppdocs.mak $target_dir/docs
cp $CPL $UIMACPP_SOURCE/docs/builddocs.sh $target_dir/docs
cp $CPL $UIMACPP_SOURCE/docs/uimacpp.dox $target_dir/docs
cp $CPL $UIMACPP_SOURCE/docs/doxyheader.html $target_dir/docs

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/src"
cp $CPL $UIMACPP_SOURCE/src/readme.tobuild $target_dir/src/
cp $CPL $UIMACPP_SOURCE/src/Makefile.in $target_dir/src/
cp $CP $UIMACPP_SOURCE/src/aclocal.m4 $target_dir/src/
cp $CP $UIMACPP_SOURCE/src/install-sh $target_dir/src/
cp $CP $UIMACPP_SOURCE/src/config.guess $target_dir/src/
cp $CP $UIMACPP_SOURCE/src/config.sub $target_dir/src/
cp $CP $UIMACPP_SOURCE/src/ltmain.sh $target_dir/src/
cp $CPL $UIMACPP_SOURCE/src/Makefile.unix $target_dir/src/
cp $CPL $UIMACPP_SOURCE/src/configure $target_dir/src/
cp $CPL $UIMACPP_SOURCE/src/base.mak $target_dir/src/

cp $CPL $UIMACPP_SOURCE/src/configure.in $target_dir/src/
cp $CPL $UIMACPP_SOURCE/src/Makefile.am $target_dir/src/

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/src/cas"
cp $CPL $UIMACPP_SOURCE/src/cas/*.cpp $target_dir/src/cas/
cp $CPL $UIMACPP_SOURCE/src/cas/uima/*.hpp $target_dir/src/cas/uima/
cp $CPL $UIMACPP_SOURCE/src/cas/uima/*.inl $target_dir/src/cas/uima/

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/src/framework"
cp $CPL $UIMACPP_SOURCE/src/framework/*.cpp $target_dir/src/framework/
cp $CPL $UIMACPP_SOURCE/src/framework/*.in $target_dir/src/framework/
cp $CPL $UIMACPP_SOURCE/src/framework/uima/*.hpp $target_dir/src/framework/uima/
cp $CPL $UIMACPP_SOURCE/src/framework/uima/*.h $target_dir/src/framework/uima/
cp $CPL $UIMACPP_SOURCE/src/framework/*.am $target_dir/src/framework/

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/src/jni"
cp $CPL $UIMACPP_SOURCE/src/jni/*.cpp $target_dir/src/jni/
cp $CPL $UIMACPP_SOURCE/src/jni/uima/*.hpp $target_dir/src/jni/uima/
cp $CPL $UIMACPP_SOURCE/src/jni/uima/*.h $target_dir/src/jni/uima/

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/src/utils"
cp $CPL $UIMACPP_SOURCE/src/utils/*.cpp $target_dir/src/utils/
cp $CPL $UIMACPP_SOURCE/src/utils/*.in $target_dir/src/utils/
cp $CPL $UIMACPP_SOURCE/src/utils/*.am $target_dir/src/utils/

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/scriptators"
cp $CPLR $UIMACPP_SOURCE/scriptators $target_dir/

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/examples"
cp $CPLR $UIMACPP_SOURCE/examples $target_dir/

print -u2 "."
print -u2 "copying from $UIMACPP_SOURCE/src/test"
cp $CPLR $UIMACPP_SOURCE/src/test $target_dir/src/
rm -rf $target_dir/src/test/.libs
rm -rf $target_dir/src/test/.deps
find $target_dir/src/test -type f -iname '*.l?' -exec rm -f {} \;
find $target_dir/src/test -type f -iname '*.o' -exec rm -f {} \;

print -u2 "."
print -u2 "removing unused files from $UIMACPP_SOURCE"
find $target_dir -depth -type d -name .svn -exec rm -rf {} \;
find $target_dir -type f -iname '*.sln' -exec rm -f {} \;
find $target_dir -type f -iname '*.vcproj' -exec rm -f {} \;
find $target_dir -type f -iname '*.cmd' -exec rm -f {} \;
find $target_dir -type f -iname '*.bat' -exec rm -f {} \;

print -u2 "."
print -u2 "DONE ... Source image created in $target_dir"

exit 0
