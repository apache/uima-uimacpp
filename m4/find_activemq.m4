# ===========================================================================
#       Derived from http://www.gnu.org/software/autoconf-archive/ax_lib_xerces.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_ICUC([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of ICU
#   particular version or newer. This macros checks for ICU
#   Parser headers and libraries and defines compilation flags
#
#   Macro supports following options and their values:
#
#     --with-activemq - path to ACTIVEMQ installation prefix 
#
#
#   This macro calls:
#
#     AC_SUBST(ACTIVEMQ_CXXFLAGS)
#     AC_SUBST(ACTIVEMQ_LDFLAGS)
#     AC_SUBST(ACTIVEMQ_VERSION) - only if version requirement is used
#
#   And sets:
#
#     icu_found
#

AC_DEFUN([AMQ_FIND_AMQ],
[
  amq_found="no"

  AC_MSG_CHECKING(for ActiveMQ 3.2 or later)
  AC_ARG_WITH(activemq,
     [ --with-activemq=PATH prefix for install ACTIVEMQ or leave out to use standard path ],
   [
      if test -d "$withval"; then
        activemq_prefix="$withval"
	activemq_install="$withval"
	AC_SUBST(activemq_install) 
      else
        AC_MSG_WARN([Invalid ACTIVEMQ path $withval])
        AC_MSG_ERROR([ACTIVEMQ not found. Please use --without-activemq or--with-activemq=PATH])
      fi
   ],
   [
      dnl Default behavior is implicit yes
      if test -x /usr/local/bin/activemqcpp-config ; then
        activemq_prefix=/usr/local
      elif test -x /usr/bin/activemqcpp-config ; then
        activemq_prefix=/usr
      else
        AC_MSG_ERROR([ACTIVEMQ not found. Please use --with-activemq= to the path])
      fi
   ])

  if test -x "$activemq_prefix/bin/activemqcpp-config"; then
    ACTIVEMQ_VERSION=`$activemq_prefix/bin/activemqcpp-config --version`
    VERSION_CHECK=`expr $ACTIVEMQ_VERSION \>\= $1`
    if test "$VERSION_CHECK" = "1"; then
      amq_found="yes"
      ACTIVEMQ_VER=`echo $activemq_prefix/include/activemq-cpp-* | sed -e 's/.*activemq-cpp-//'`
      ACTIVEMQ_CXXFLAGS="-I$activemq_prefix/include/activemq-cpp-$ACTIVEMQ_VER"
#     ACTIVEMQ_CXXFLAGS=`$activemq_prefix/bin/activemqcpp-config --exec-prefix=$activemq_prefix --include`
      ACTIVEMQ_LDFLAGS=`$activemq_prefix/bin/activemqcpp-config --exec-prefix=$activemq_prefix --libs`
      AC_SUBST(ACTIVEMQ_VERSION) 
      AC_SUBST(ACTIVEMQ_CXXFLAGS)   
      AC_SUBST(ACTIVEMQ_LDFLAGS)
    else 
      AC_MSG_ERROR([Invalid activemq version $ACTIVEMQ_VERSION found. activemq version $1 or higher is required]) 
    fi  
  else
    AC_MSG_ERROR([activemqcpp-config script not found in $activemq_prefix/bin])
  fi
 
  AC_MSG_RESULT($amq_found)

])
