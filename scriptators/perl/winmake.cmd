@echo off
if "%UIMACPP_HOME%" == "" goto TafRootNotSet
if not exist "%UIMACPP_HOME%"\include\uima\cas.hpp goto UimaRootBad

set SWIG_PATH=\swigwin-1.3.29
if not exist %SWIG_PATH% goto SwigPathNotSet

set PERL5_INCLUDE=\Perl\lib\CORE
set PERL5_LIB=\Perl\lib\CORE
set PERL5_LIBDIR=\Perl\lib\CORE
if not exist %PERL5_LIB% goto PerlNotSet

REM build Perltator
devenv perltator.vcproj /build release 
goto TheEnd

:UimaRootBad
echo UIMACPP_HOME is invalid
goto TheEnd

:SwigPathNotSet
echo SWIG_PATH is invalid
goto TheEnd

:PerlNotSet
echo Perl environment is invalid


:TheEnd
