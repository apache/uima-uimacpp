#!/bin/sh
#REM  Builds the UIMACPP doxygen docs.
#REM	Requires Doxygen 1.3.6 and Graphviz 1.8.10 installed and in the PATH
#REM  This script must be run from the uimacpp/docs subdirectory

if [ -z "$1" ] ; then
    cmd="build"
    echo "command is $cmd"
else
    cmd=$1
fi
if [ $cmd  = "build" ] ; then 
	echo "building docs... "
	make -f uimacppdocs.mak build DEL=rm RD='rm -rf' MDFILES=../docs/html/*.md5  MAPFILES=../docs/html/*.map DOCDIR=../docs 
elif [ $cmd = "rebuild" ] ; then
	echo "rebuilding docs..."
	make -f uimacppdocs.mak rebuild DEL=rm  RD='rm -rf' MDFILES=../docs/html/*.md5  MAPFILES=../docs/html/*.map DOCDIR=../docs
elif [ $cmd = "clean" ] ; then
	echo "cleaning docs dir..."
	make -f uimacppdocs.mak clean DEL=rm  RD='rm -rf' DOCDIR=../docs
fi
