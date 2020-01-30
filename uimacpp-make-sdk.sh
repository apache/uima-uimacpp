#!/bin/sh

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

#
# 1. Download and build all uimacpp 3rd party library dependencies
# 2. Build uimacpp and run test cases
# 3. Build uimacpp docs
# 4. Assemble uimacpp SDK tree
#

set -e

PREFIX=`pwd`/dependencies
TARGET=`pwd`/target

mkdir -p "$PREFIX"
mkdir -p "$TARGET"

#guess JAVA_HOME if not set
if [ -z $JAVA_HOME ]; then
    JAVA_HOME=$(dirname $(dirname $(dirname $(readlink -f $(which java)))))
    if [ ! -f "$JAVA_HOME/include/jni.h" ]; then
	echo Cannot guess JAVA_HOME. JAVA SDK installed?
	exit
    fi
    echo guessing JAVA_HOME=$JAVA_HOME
else
    echo using specified JAVA_HOME=$JAVA_HOME
fi

# Install location for 3rd party components
export APR_HOME=$PREFIX
export APU_HOME=$PREFIX
export XERCES_HOME=$PREFIX
export ICU_HOME=$PREFIX
export AMQ_HOME=$PREFIX

# 3rd party component versons to use
XERCES=xerces-c-3.1.4
ICU=icu4c-50_2-src.tgz
APR=apr-1.4.8
APRUTIL=apr-util-1.6.1
AMQCPP=activemq-cpp-library-3.9.3
# Additional modifications may be needed below for changes to Xerces, ICU, or ActiveMQ-C++

# Build xerces
if [ ! -f "${XERCES}.tar.gz" ]; then
    wget http://archive.apache.org/dist/xerces/c/3/sources/${XERCES}.tar.gz
    tar -xf ${XERCES}.tar.gz
fi
#Modify next line if not version 3.1.x
if [ ! -f "$XERCES/src/.libs/libxerces-c-3.1.so" ]; then
    cd $XERCES
    ./configure --prefix="$PREFIX"
    make install
    cd ..
else
    echo Xerces previously built
fi

# Build ICU
if [ ! -f $ICU ]; then
#Modify next line if not version 50.2
    wget https://github.com/unicode-org/icu/releases/download/release-50-2/$ICU
    tar -xzf $ICU
fi
if [ ! -f icu/source/bin/uconv ]; then
    cd icu/source
    ./runConfigureICU Linux --prefix="$PREFIX"
    make install
    cd ../..
else
    echo ICU previously built
fi

# Build APR
if [ ! -f "${APR}.tar.gz" ]; then
    wget http://archive.apache.org/dist/apr/${APR}.tar.gz
    tar -xzf ${APR}.tar.gz
fi
if [ ! -f "$APR/build/apr_rules.out" ]; then
    cd $APR
    ./configure --prefix="$PREFIX"
    make install
    cd ..
else
    echo APR previously built
fi

# Build APR-UTIL
if [ ! -f "${APRUTIL}.tar.gz" ]; then
    wget http://mirrors.ocf.berkeley.edu/apache/apr/${APRUTIL}.tar.gz
    tar -xf ${APRUTIL}.tar.gz
fi
if [ ! -f ${APRUTIL}/apu-config.out ]; then
    cd $APRUTIL
    ./configure --prefix=$PREFIX --with-apr=$PREFIX/bin/apr-1-config
    make install
    cd ..
else
    echo "APR-UTIL previously built"
fi

# Build ActiveMQ-C++
if [ ! -f ${AMQCPP}-src.tar.gz ]; then
#Modify next line if not v3.9.3
    wget http://archive.apache.org/dist/activemq/activemq-cpp/3.9.3/${AMQCPP}-src.tar.gz
    tar -xf ${AMQCPP}-src.tar.gz
fi
if [ ! -f ${AMQCPP}/src/main/.libs/libactivemq-cpp.so ]; then
    cd $AMQCPP
    ./configure --prefix=$PREFIX --with-apr=$PREFIX/bin/apr-1-config
    make install
    cd ..
else
    echo "ActiveMQ C++" previously built
fi

# Now build UIMA

./autogen.sh
./configure --prefix=$TARGET --with-xerces=$PREFIX --with-apr=$PREFIX --with-apr-util=$PREFIX --with-icu=$PREFIX --with-activemq=$PREFIX --with-jdk=$JAVA_HOME/include' -I'${JAVA_HOME}/include/linux CXXFLAGS=-std=c++11
make check
make install
make docs
make sdk TARGETDIR=sdk
