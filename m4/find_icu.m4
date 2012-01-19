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
#     --with-icu - path to ICU installation prefix
#
#
#   This macro calls:
#
#     AC_SUBST(ICU_CPPFLAGS)
#     AC_SUBST(ICU_LDFLAGS)
#     AC_SUBST(ICU_VERSION) - only if version requirement is used
#
#   And sets:
#
#     icu_found
#

AC_DEFUN([ICU_FIND_ICU],
[
  icu_found="no"

  AC_MSG_CHECKING(for ICU)
  AC_ARG_WITH(icu,
     [ --with-icu=PATH prefix for install ICU or will try standar path ],
   [
      if test -d "$withval"; then
        icu_prefix="$withval"
	icu_install="$withval"
        AC_SUBST(icu_install)
      else
        AC_MSG_ERROR([Invalid ICU path $withval])
      fi
   ],
   [
      dnl Default behavior is implicit yes
      if test -d /usr/local/include/unicode ; then
        icu_prefix=/usr/local
      elif test -d /usr/include/unicode ; then
        icu_prefix=/usr
      else
        AC_MSG_ERROR([ICU not found. Please use --with-icu= to the path])
      fi
   ])

  icu_found="yes"

  if test -x "$icu_prefix/bin/icu-config"; then
    ICU_VERSION=`$icu_prefix/bin/icu-config --prefix=$icu_prefix --version`
    VERSION_CHECK=`expr $ICU_VERSION \>\= $1`
    if test "$VERSION_CHECK" = "1"; then
      ICU_CPPFLAGS="-I$icu_prefix/include"
      ICU_LDFLAGS="-L$icu_prefix/lib -licui18n -licuuc -licuio -licudata"
      AC_SUBST(ICU_CPPFLAGS)
      AC_SUBST(ICU_LDFLAGS)
      AC_MSG_RESULT($icu_found)
    else
      icu_found="no"
      AC_MSG_ERROR([Invalid ICU version $ICU_VERSION found. ICU version $1 or higher is required])
    fi 
  else
   icu_found="no"
   AC_MSG_ERROR([icu_config script not found.])
  fi

])
