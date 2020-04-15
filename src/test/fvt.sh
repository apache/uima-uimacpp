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

if [ ! $UIMACPP_HOME ]
then
   export UIMACPP_HOME=../..
fi

 Testlib=.libs:../framework/.libs
 Testbin=.libs

export LD_LIBRARY_PATH=$Testlib:$APR_HOME/lib:$ICU_HOME/lib:$XERCES_HOME/lib:$UIMACPP_HOME/lib
# OSX does not allow DYLD_LIBRARY_PATH to be propagated in the environment
export OSX_LIBRARYPATH=$Testlib:$APR_HOME/lib:$ICU_HOME/lib:$XERCES_HOME/lib:$UIMACPP_HOME/lib:$UIMACPP_HOME/dependencies/lib
export PATH=$Testbin:$PATH

export UIMACPP_DATAPATH=./data

if [ ! $TEMP ]
then
  export TEMP=/tmp/$USER
  mkdir -p $TEMP
fi

if [ "$2" = "debug" ]; then
  export DEBUG_FVT="gdb $Testbin/"
fi
if [ ! $UIMACPP_STREAMHANDLERS ]
then
   export UIMACPP_STREAMHANDLERS=file:libSofaStreamHandlerFile
fi

export RM=rm

if [ $UIMA_HOME ]
then
    export UIMACPPTEST_JNI="java -Duima.home=$UIMA_HOME -Duima.datapath=$UIMA_DATAPATH -Djava.util.logging.config.file=$UIMA_HOME/config/Logger.properties -Duima.use_jul_as_default_uima_logger -DNoOp -DUimaBootstrapSuppressClassPathDisplay -Dorg.apache.uima.jarpath=$UIMA_HOME/lib -jar $UIMA_HOME/lib/uimaj-bootstrap.jar org.apache.uima.tools.RunAE data/descriptors/javaaggregate.xml data/segmenterinput"
else
  export UIMACPPTEST_JNI="@echo UIMA_HOME is not set. JNI test was not run."
fi

# On Mac OSX if the installed java is protected the DYLD_LIBRARY_PATH environment variable is not
# passed to the JNI test and the UIMA libraries will fail to load.
# Instead use a private java or disable SIP.
UNAME=`uname -s`
if [ "$UNAME" = "Darwin" ]
then
	echo "***"
	echo "*** WARNING *** The JNI test may fail if using an installed java and System Integrity Protection is enabled"
	echo "***"
fi

make -f fvtTestfile $1




