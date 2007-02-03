#
if [ ! $UIMACPP_HOME ]
then
   export UIMACPP_HOME=../..
fi

# If appears to be the "install" place use its lib & bin (stripped!)
# If appears to be the build place use the unstripped programs

if [ -d $UIMACPP_HOME/src ] 
then
 Testlib=$UIMACPP_HOME/src/test/.libs:$UIMACPP_HOME/src/framework/.libs
 Testbin=$UIMACPP_HOME/src/test/.libs
else 
 Testlib=$UIMACPP_HOME/lib
 Testbin=$UIMACPP_HOME/bin
fi

export LD_LIBRARY_PATH=$Testlib:$APR_HOME/lib:$ICU_HOME/lib:$XERCES_HOME/lib
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
export UIMACPPTEST_JNI="$UIMA_HOME/bin/runAE.sh data/descriptors/javaaggregate.xml data/segmenterinput"
if [ ! $UIMA_HOME ] 
then
  export UIMACPPTEST_JNI="@echo UIMA_HOME is not set. JNI test was not run."
fi

make -f fvtTestfile $1




