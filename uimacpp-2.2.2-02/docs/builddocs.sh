# ---------------------------------------------------------------------------
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ---------------------------------------------------------------------------

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
