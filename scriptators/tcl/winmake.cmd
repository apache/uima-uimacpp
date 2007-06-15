@echo off
if "%UIMACPP_HOME%" == "" goto TafRootNotSet
if not exist "%UIMACPP_HOME%"\include\uima\cas.hpp goto UimaRootBad

set SWIG_PATH=\swigwin-1.3.29
if not exist %SWIG_PATH% goto SwigPathNotSet

set TCL_INCLUDE=\Tcl\include
set TCL_LIBDIR=\Tcl\lib
if not exist %TCL_LIBDIR% goto TclNotSet

REM build Tclnator
devenv tclator.vcproj /build release 
goto TheEnd

:UimaRootBad
echo UIMACPP_HOME is invalid
goto TheEnd

:SwigPathNotSet
echo SWIG_PATH is invalid
goto TheEnd

:TclNotSet
echo Tcl environments is invalid


:TheEnd
