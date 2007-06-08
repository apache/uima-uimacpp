@echo off
setlocal
REM Must be run from the uimacpp/src/test directory

if "%UIMACPP_HOME%"=="" (
  set UIMACPP_HOME=..\..\install
)

REM default location of descriptors 
if "%UIMACPP_DATAPATH%"=="" set UIMACPP_DATAPATH=.\data

REM add location of apr, icu, xerces and uimacpp binaries to the path 
set PATH=%UIMACPP_HOME%\bin;%APR_HOME%\Release;%ICU_HOME%\bin;%XERCES_HOME%\bin;%PATH%

set RM=\windows\system32\cmd /C del

if "%~2"=="debug" (
  set DEBUG_FVT="devenv /debugexe "
)
if "%UIMACPP_STREAMHANDLERS%"=="" set UIMACPP_STREAMHANDLERS=file:SofaStreamHandlerFile

set UIMACPPTEST_JNI=call "%UIMA_HOME%\bin\runAE" data\descriptors\javaaggregate.xml data\segmenterinput

if "%UIMA_HOME%" == "" (
	set UIMACPPTEST_JNI=@echo "UIMA_HOME is not set. The JNI test was not run."
)

if "%~1" == "debug" (
  nmake -f fvtTestfile.debug %2
)
if "%~2" == "debug" (
  nmake -f fvtTestfile.debug %1
)
if not "%~2" == "debug" (
  if not "%~1" == "debug" (
    nmake -f fvtTestfile %1
  )
)

goto end

:end
