
 # Licensed to the Apache Software Foundation (ASF) under one
 # or more contributor license agreements.  See the NOTICE file
 # distributed with this work for additional information
 # regarding copyright ownership.  The ASF licenses this file
 # to you under the Apache License, Version 2.0 (the
 # "License"); you may not use this file except in compliance
 # with the License.  You may obtain a copy of the License at
 # 
 #   http://www.apache.org/licenses/LICENSE-2.0
 # 
 # Unless required by applicable law or agreed to in writing,
 # software distributed under the License is distributed on an
 # "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 # KIND, either express or implied.  See the License for the
 # specific language governing permissions and limitations
 # under the License.

AC_INIT(uimacpp, 3.0.0)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/framework/engine.cpp)
AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_MACRO_DIR([m4])
AC_DISABLE_STATIC
AC_PREFIX_DEFAULT(/usr/local/uimacpp)

#[LIB_VERSION=`echo $VERSION | sed -e 's/\.//' -e 's/^\([^.]*\)$/\1.0/'`]
#LIB_VERSION_MAJOR=`echo $LIB_VERSION | sed 's/\..*//'`

AC_ARG_ENABLE(debuguima,
[  --enable-debuguima    Turn on debugging],
[case "${enableval}" in
  yes) debuguima=true ;;
  no)  debuguima=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-debuguima) ;;
esac],[debuguima=false])
AM_CONDITIONAL([DEBUG_UIMA], [test x$debuguima = xtrue])


AC_ARG_ENABLE(debugtest,
[  --enable-debugtest    Turn on debugging],
[case "${enableval}" in
  yes) debugtest=true ;;
  no)  debugtest=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-debugtest) ;;
esac],[debugtest=false])
AM_CONDITIONAL([DEBUG_TEST], [test x$debugtest = xtrue])

LT_VERSION_NUMBER=3:0:0

AC_SUBST(LT_VERSION_NUMBER)
AC_SUBST(VERSION)
AC_MSG_RESULT([release $VERSION, library $LIB_VERSION])

AC_PROG_LIBTOOL
AC_PROG_CXX

AC_C_BIGENDIAN

######################################
# setup for using default dependencies
UIMACPP_HOME_INCLUDE="-I$UIMACPP_HOME/include"
UIMACPP_HOME_LIB="-L$UIMACPP_HOME/lib"

#########################################################################
#
# new check for XercesC
#

AC_MSG_NOTICE([Xerces C++ library configuration])
AX_LIB_XERCES([2.8])

if test $HAVE_XERCES = "no"; then
  AC_MSG_WARN([XERCES not found])
  echo "The Xerces C++ library cannot be found."
  echo "Please install XERCES on this system and supply the appropriate"
  echo "--with-xerces option to 'configure'"

  AC_MSG_ERROR([no suitable XERCES found])
fi

echo "Found $XERCES_VERSION $XERCES_CPPFLAGS $XERCES_LDFLAGS $XERCES_LIBS"
# Get build information from XERCES
UIMA_XERCES_INCLUDE="$XERCES_CPPFLAGS"
UIMA_XERCES_LIB="$XERCES_LDFLAGS $XERCES_LIBS"

AC_SUBST(UIMA_XERCES_INCLUDE)
AC_SUBST(UIMA_XERCES_LIB)

#########################################################################
#
# check for XercesC
#
CPPFLAGS_save="$CPPFLAGS"
#CPPFLAGS="$CPPFLAGS $XERCES_TEST_INCLUDE"
CPPFLAGS="$CPPFLAGS $UIMA_XERCES_INCLUDE"
AC_CHECK_HEADER([xercesc/util/XercesVersion.hpp],
                [:],
                [AC_MSG_ERROR([Unable to find Xerces C++ headers])])
CPPFLAGS="$CPPFLAGS_save"


#########################################################################
#
# new check for APR 
#

AC_MSG_NOTICE([Apache Portable Runtime (APR) library configuration])
APR_FIND_APR([], [], 1, [1])

if test $apr_found = "no"; then
  AC_MSG_WARN([APR not found])
  echo "The Apache Portable Runtime (APR) library cannot be found."
  echo "Please install APR on this system and supply the appropriate"
  echo "--with-apr option to 'configure'"

  AC_MSG_ERROR([no suitable APR found])
fi

# check APR version number agains regex
#APR_VER_REGEXES=["1\.3\.[0-9] 1\.3\.1[0-9] 1\.4\.[0-9]"]
APR_VER_REGEXES=["1\.4\.[0-9] 1\.5\.[0-9] 1\.6\.[0-9]"]

AC_MSG_CHECKING([APR version])
apr_version="`$apr_config --version`"
if test $? -ne 0; then
  AC_MSG_ERROR([apr-config --version failed])
fi
AC_MSG_RESULT([$apr_version])

APR_WANTED_REGEX_MATCH=0
for apr_wanted_regex in $APR_VER_REGEXES; do
  if test `expr $apr_version : $apr_wanted_regex` -ne 0; then
    APR_WANTED_REGEX_MATCH=1
    break
  fi
done

if test $APR_WANTED_REGEX_MATCH -eq 0; then
  echo "wanted regexes are $APR_VER_REGEXES"
  AC_MSG_ERROR([invalid apr version found, check the README for supported versions.])
fi

# Get build information from APR


APR_CPPFLAGS="`$apr_config --cppflags`"
if test $? -ne 0; then
  AC_MSG_ERROR([apr-config --cppflags failed])
fi

APR_INCLUDES="`$apr_config --includes`"
if test $? -ne 0; then
  AC_MSG_ERROR([apr-config --includes failed])
fi

APR_LDFLAGS="`$apr_config --ldflags`"
if test $? -ne 0; then
  AC_MSG_ERROR([apr-config --ldflags failed])
fi

APR_LIBS="`$apr_config --link-libtool --libs`"
if test $? -ne 0; then
  AC_MSG_ERROR([apr-config --link-libtool --libs failed])
fi

UIMA_APR_INCLUDE="$APR_CPPFLAGS $APR_INCLUDES"
UIMA_APR_LIB="$APR_LDFLAGS $APR_LIBS"

#AC_SUBST([APR_LIBS])
#AC_SUBST([APR_LDFLAGS])
#AC_SUBST([APR_CPPFLAGS])
#AC_SUBST([APR_INCLUDES])

AC_SUBST(UIMA_APR_INCLUDE)
AC_SUBST(UIMA_APR_LIB)

#########################################################################
#
# check for APR
#

CPPFLAGS_save="$CPPFLAGS"
#CPPFLAGS="$CPPFLAGS $APR_TEST_INCLUDE -D_GNU_SOURCE"
CPPFLAGS="$CPPFLAGS $UIMA_APR_INCLUDE -D_GNU_SOURCE"
AC_CHECK_HEADER([apr.h],
                [:],
                [AC_MSG_ERROR([Unable to find APR Headers])])
CPPFLAGS="$CPPFLAGS_save"
#########################################################################
#
# check for ICU
#
AC_MSG_NOTICE([ICU  C++ library configuration])
ICU_FIND_ICU([3.2])

if test $icu_found = "no"; then
  AC_MSG_WARN([ICU not found])
  echo "The ICU C++ library cannot be found."
  echo "Please install ICU on this system and supply the appropriate"
  echo "--with-icu option to 'configure'"

  AC_MSG_ERROR([no suitable ICU found])
fi

# Get build information from XERCES
UIMA_ICU_INCLUDE="$ICU_CPPFLAGS"
UIMA_ICU_LIB="$ICU_LDFLAGS $XERCES_LIBS"

AC_SUBST(UIMA_ICU_INCLUDE)
AC_SUBST(UIMA_ICU_LIB)



#########################################################################
#
# check for ICU
#

CPPFLAGS_save="$CPPFLAGS"
#CPPFLAGS="$CPPFLAGS $ICU_TEST_INCLUDE"
CPPFLAGS="$CPPFLAGS $UIMA_ICU_INCLUDE"
AC_CHECK_HEADER([unicode/uversion.h],
                [:],
                [AC_MSG_ERROR([Unable to find ICU Headers])])
CPPFLAGS="$CPPFLAGS_save"


########################################################################
# check for ActiveMQ new
#
activemq=true
AC_ARG_WITH([activemq],
           [AS_HELP_STRING([--without-activemq],
             [disable support for ActiveMQ service wrapper])],
            [],
            [with_activemq=check])

if test "x$with_activemq" = xno; then
  activemq=false
  echo "disabling service wrapper"
else
  AC_MSG_NOTICE([ActiveMQ  C++ library configuration])
  AMQ_FIND_AMQ([3.2])

  if test $amq_found = "no"; then
    AC_MSG_WARN([ACTIVEMQ not found])
    echo "The ACTIVEMQ C++ library cannot be found."
    echo "Please install ACTIVEMQ 3.2 or higher on this system and supply the appropriate"
    echo "--with-activemq option to 'configure'"

    AC_MSG_ERROR([no suitable ACTIVEMQ found])
  fi

  # Get build information from ACTIVEMQ
  UIMA_ACTIVEMQ_INCLUDE="$ACTIVEMQ_CXXFLAGS"
  UIMA_ACTIVEMQ_LIB="$ACTIVEMQ_LDFLAGS"

  AC_SUBST(UIMA_ACTIVEMQ_INCLUDE)
  AC_SUBST(UIMA_ACTIVEMQ_LIB)
  CPPFLAGS_save="$CPPFLAGS"
  CPPFLAGS="$CPPFLAGS $UIMA_ACTIVEMQ_INCLUDE"
  activemq=true
  AC_CHECK_HEADER([activemq/exceptions/ExceptionDefines.h],
                [:],
                [activemq=false])
  CPPFLAGS="$CPPFLAGS_save"
  if test "x$activemq" = "xfalse"; then
    AC_MSG_ERROR([ActiveMQ Headers not found. Specify valid path to ActiveMQ.])
  fi
  AC_MSG_NOTICE([Apache Portable Runtime (APR Util ) library configuration])
  APR_FIND_APU([], [], 1, [1])

  if test $apu_found = "no"; then
    AC_MSG_WARN([APR-Util not found])
    echo "APR-Util is required if building with ActiveMQ."
    echo "Please install APR-Util on this system and supply the appropriate"
    echo "--with-apr-util option to 'configure'"
    AC_MSG_ERROR([no suitable APR-Util found. Required to build with ActiveMQ. Rerun configure with the option --with-apr-util=PATH ])
fi

fi 

#########################################################################
#
# check for ACTIVEMQ
#

#CPPFLAGS_save="$CPPFLAGS"
##CPPFLAGS="$CPPFLAGS $ACTIVEMQ_TEST_INCLUDE"
#CPPFLAGS="$CPPFLAGS $UIMA_ACTIVEMQ_INCLUDE"
#activemq=true
#AC_CHECK_HEADER([activemq/exceptions/ExceptionDefines.h],
#                [:],
#                [activemq=false])

#CPPFLAGS="$CPPFLAGS_save"


AM_CONDITIONAL([ACTIVEMQ_FOUND], [test x$activemq = xtrue])
if test "x$activemq" = "xfalse"; then
AC_MSG_RESULT(***ACTIVEMQ dependency not found)
AC_MSG_RESULT(***Not building deployCppService)
AC_MSG_RESULT(***To try again ... touch configure before running make)
fi



####################################
#Substitute default paths, if needed
AC_SUBST(UIMACPP_HOME_INCLUDE)
AC_SUBST(UIMACPP_HOME_LIB)


#########################################################################
#
# check for Java JDK
#

AC_ARG_WITH([jdk], AC_HELP_STRING([--with-jdk],
                                      [use local JDK (uses /opt/IBMJava2-142/include by default)]),
               [UIMA_JDK_INCLUDE="-I$withval"
                JDK_TEST_INCLUDE="$UIMA_JDK_INCLUDE"],
                    [UIMA_JDK_INCLUDE="-I/opt/IBMJava2-142/include"
                     JDK_TEST_INCLUDE="-I/opt/IBMJava2-142/include"])

CPPFLAGS_save="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $JDK_TEST_INCLUDE"
AC_CHECK_HEADER([jni.h],
                [:],
                [AC_MSG_ERROR([Unable to find JDK Headers])])
CPPFLAGS="$CPPFLAGS_save"

AC_SUBST(UIMA_JDK_INCLUDE)


#########################################################################
#
# setup for link with UIMA lib
#

UIMA_LIB="-L\$(top_srcdir)/src/framework -luima"
AC_SUBST(UIMA_LIB)

AM_CONDITIONAL(APPLE, test x$build_vendor = xapple)

AC_OUTPUT(Makefile src/Makefile src/framework/Makefile src/utils/Makefile src/test/Makefile)
