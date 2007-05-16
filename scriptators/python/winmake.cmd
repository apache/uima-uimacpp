@echo off
if "%UIMACPP_HOME%" == "" goto TafRootNotSet
if not exist %UIMACPP_HOME%\include\uima\cas.hpp goto UimaRootBad

set SWIG_PATH=\swigwin-1.3.29
if not exist %SWIG_PATH% goto SwigPathNotSet

set PYTHON_INCLUDE=\Python24\include
set PYTHON_LIB=\Python24\lib
set PYTHON_LIBDIR=\Python24\libs
if not exist %PYTHON_LIB% goto PythonNotSet

REM build Pythonnator
devenv pythonnator.vcproj /build release 
goto TheEnd

:UimaRootBad
echo UIMACPP_HOME is invalid
goto TheEnd

:SwigPathNotSet
echo SWIG_PATH is invalid
goto TheEnd

:PythonNotSet
echo Python environment is invalid


:TheEnd
