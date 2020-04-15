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

# with any command line arg
# 1. Download and build minimum required 3rd party library dependencies
# 2. Build uimacpp library and run test cases
#
# else
# 1. Download and build all uimacpp 3rd party library dependencies
# 2. Build uimacpp and run test cases
# 3. Build uimacpp docs
# 4. Assemble uimacpp SDK tree
#

set -e

# test only if any argument set
if [ "$#" -gt 0 ]; then
   TESTONLY="true"
fi

PREFIX=`pwd`/dependencies
TARGET=`pwd`/target

mkdir -p "$PREFIX"
mkdir -p "$TARGET"

# Linux / Mac OSX configuration customization for AMQ build
# If ActiveMQ falsely believes OpenSSL is installed use: AMQARG=--disable-ssl
# On Mac OpenSSL is usually installed in /usr/local/opt/openssl

UNAME=`uname -s`
if [ "$UNAME" = "Darwin" ]; then
	LIBEXT=dylib
	INCDIR=darwin
	ICUARG=MacOSX
	AMQARG=--with-openssl=/usr/local/opt/openssl
else
	LIBEXT=so
	INCDIR=linux
	ICUARG=Linux
	AMQARG=
fi

#guess JAVA_HOME if not set
if [ -z $JAVA_HOME ]; then
	if [ "$UNAME" = "Darwin" ]; then
	    JAVA_HOME=$(/usr/libexec/java_home)
	else
	    JAVA_HOME=$(dirname $(dirname $(readlink -f $(which java))))
	    if [ ! -f "$JAVA_HOME/include/jni.h" ]; then
		JAVA_HOME=$(dirname $(dirname $(dirname $(readlink -f $(which java)))))
	    fi
	fi
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
export UIMA_HOME=$PWD/apache-uima

# 3rd party component versons to use
UIMAJ=uimaj-3.1.0
XERCESMAJOR=3
XERCES=xerces-c-3.2.3
ICURELEASE=release-65-1
ICU=icu4c-65_1-src.tgz
APR=apr-1.7.0
APRUTIL=apr-util-1.6.1
AMQVERSION=3.9.5
AMQCPP=activemq-cpp-library-$AMQVERSION

# Install uimaj for JNI test
if [ ! -f "${UIMAJ}-bin.tar.gz" ]; then
    echo Installing UIMAJ
    wget http://archive.apache.org//dist/uima//${UIMAJ}/${UIMAJ}-bin.tar.gz
    tar -xf ${UIMAJ}-bin.tar.gz
else
    echo uimaj already installed
fi

# Build xerces
if [ ! -f "${XERCES}.tar.gz" ]; then
    wget http://archive.apache.org/dist/xerces/c/$XERCESMAJOR/sources/${XERCES}.tar.gz
    tar -xf ${XERCES}.tar.gz
fi
#Check if already built
if [ ! -f "$XERCES/src/.libs/libxerces-c.$LIBEXT" ]; then
    cd $XERCES
	echo Building Xerces
    ./configure --prefix="$PREFIX" --enable-netaccessor-socket
    make install
    cd ..
else
    echo Xerces previously built
fi

# Build ICU
if [ ! -f $ICU ]; then
    wget https://github.com/unicode-org/icu/releases/download/$ICURELEASE/$ICU
    tar -xzf $ICU
fi
if [ ! -f icu/source/bin/uconv ]; then
    cd icu/source
	echo Building ICU
    ./runConfigureICU $ICUARG --prefix="$PREFIX"
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
    echo Building APR
    ./configure --prefix="$PREFIX"
    make install
    cd ..
else
    echo APR previously built
fi

if [ -z $TESTONLY ]; then
    # Build APR-UTIL
    if [ ! -f "${APRUTIL}.tar.gz" ]; then
        wget http://mirrors.ocf.berkeley.edu/apache/apr/${APRUTIL}.tar.gz
        tar -xf ${APRUTIL}.tar.gz
    fi
    if [ ! -f ${APRUTIL}/apu-config.out ]; then
        cd $APRUTIL
        echo Building APR-UTIL
        ./configure --prefix=$PREFIX --with-apr=$PREFIX/bin/apr-1-config
        make install
        cd ..
    else
        echo "APR-UTIL previously built"
    fi

    # Build ActiveMQ-C++
    if [ ! -f ${AMQCPP}-src.tar.gz ]; then
        wget http://archive.apache.org/dist/activemq/activemq-cpp/${AMQVERSION}/${AMQCPP}-src.tar.gz
        tar -xf ${AMQCPP}-src.tar.gz
    fi
    if [ ! -f ${AMQCPP}/src/main/.libs/libactivemq-cpp.${LIBEXT} ]; then
        cd $AMQCPP
        echo Building ActiveMQ
        ./configure $AMQARG --prefix=$PREFIX --with-apr=$PREFIX/bin/apr-1-config
        make install
        cd ..
    else
        echo "ActiveMQ C++" previously built
    fi
fi

# Now build UIMA

echo Building UIMA
./autogen.sh
if [ -z $TESTONLY ]; then
    ./configure --prefix=$TARGET --with-xerces=$PREFIX --with-apr=$PREFIX --with-apr-util=$PREFIX --with-icu=$PREFIX --with-activemq=$PREFIX --with-jdk=$JAVA_HOME/include' -I'${JAVA_HOME}/include/${INCDIR} CXXFLAGS=-std=c++11
    make check
    make install
    make docs
    rm -rf sdk
    make sdk TARGETDIR=sdk
else
    ./configure --prefix=$TARGET --with-xerces=$PREFIX --with-apr=$PREFIX --with-icu=$PREFIX --without-activemq --with-jdk=$JAVA_HOME/include' -I'${JAVA_HOME}/include/${INCDIR} CXXFLAGS=-std=c++11
    make check
fi
