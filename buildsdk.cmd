@ECHO OFF
SETLOCAL

REM   Licensed to the Apache Software Foundation (ASF) under one
REM   or more contributor license agreements.  See the NOTICE file
REM   distributed with this work for additional information
REM   regarding copyright ownership.  The ASF licenses this file
REM   to you under the Apache License, Version 2.0 (the
REM   "License"); you may not use this file except in compliance
REM   with the License.  You may obtain a copy of the License at
REM
REM    http://www.apache.org/licenses/LICENSE-2.0
REM
REM   Unless required by applicable law or agreed to in writing,
REM   software distributed under the License is distributed on an
REM   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
REM   KIND, either express or implied.  See the License for the
REM   specific language governing permissions and limitations
REM   under the License.

REM check args
if "%~1" == "" (
	echo ERROR: SDK target directory not specified
	echo ------
	echo   Usage: buildsdk targetDirectory [clean]
	echo     Builds SDK for distribution.
	echo     buildsdk must be run from the root of the uimacpp source tree.
	echo     The uimacpp solution must be built and installed by running
	echo       winmake in the src directory of the uimacpp source tree. 
	echo     The doxygen documentation must be built by running the buildocs.cmd
	echo       in the docs directory of the uimacpp source tree.
	echo     Requires the following environment variables:
	echo       APR_HOME - root of the APR install. Required.
	echo       ICU_HOME - root of the ICU install. Required.
	echo       XERCES_HOME - root of the XERCES install. Required.
	echo       MSVCRT_HOME - directory with required msvc*.dll files
	echo       APU_HOME - root of the APR Util install Required if ACTIVEMQ_HOME is set.
	echo       ACTIVEMQ_HOME - root of the XERCES install. Optional.
	echo     Optional environment variable:
	echo       UIMA_INSTALL - 'install' location of uimacpp build.
	echo                      Defaults to ./install
	goto error
)

set TARGET_DIR=%~1%

if not exist %TARGET_DIR% mkdir %TARGET_DIR%
if not exist %TARGET_DIR% (
	echo ERROR: Could not create %TARGET_DIR%
	goto error
)
set UIMA_DIR=%TARGET_DIR%\uimacpp


if "%APR_HOME%" == "" goto Missing
if "%ICU_HOME%" == "" goto Missing
if "%XERCES_HOME%" == "" goto Missing
if not "%ACTIVEMQ_HOME%" == "" (
  if "%APU_HOME%" == "" goto MissingAprUtil

)

echo.
echo SDK directory tree will be built in %UIMA_DIR%
echo.
REM check environment values and set default values
set UIMA_SOURCE=%cd%
if "%UIMA_INSTALLDIR%"=="" set UIMA_INSTALLDIR=%UIMA_SOURCE%\install
if "%UIMA_DOCDIR%" == "" set UIMA_DOCDIR=%UIMA_SOURCE%\docs
if "%UIMA_EXAMPLESDIR%" == "" set UIMA_EXAMPLESDIR=%UIMA_SOURCE%\examples
if "%UIMA_SCRIPTATORSDIR%" == "" set UIMA_SCRIPTATORSDIR=%UIMA_SOURCE%\scriptators
REM if "%UIMA_LICENSEDIR%" == "" set UIMA_LICENSEDIR=%UIMA_SOURCE%\licenses
if "%UIMA_TESTSRCDIR%" == "" set UIMA_TESTSRCDIR=%UIMA_SOURCE%\src\test\src

REM if not exist "%UIMA_INSTALLDIR%"\bin\runAECpp.exe goto uimaInstallPathInvalid
if not exist "%UIMA_INSTALLDIR%"\include\uima\api.hpp (
	echo ERROR: UIMA_INSTALLDIR "%UIMA_INSTALLDIR%" is invalid.
	echo Build and install UIMA C++ first. 
	echo 	devenv src\uimacpp.sln
	goto error
)

if not exist "%UIMA_DOCDIR%"\html\index.html (
	echo ERROR: UIMACPP doxygen docs not found in %UIMA_DOCDIR%
	echo run the builddocs script in the %UIMA_DOCDIR% directory. 
	goto error
)

if not exist "%APR_HOME%"\include (
	echo ERROR: APR_HOME "%APR_HOME%" is invalid.
	goto error
)

if not exist "%ICU_HOME%"\include (
	echo ERROR: ICU_HOME "%ICU_HOME%" is invalid.
	goto error
)

if not exist "%XERCES_HOME%"\include (
	echo ERROR: XERCES_HOME "%XERCES_HOME%" is invalid.
	goto error
)

if "%MSVCRT_HOME%" == "" (
        set MSVCRT_HOME=C:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT
        echo MSVCRT_HOME undefined: trying: "%MSVCRT_HOME%"
)

if not exist "%MSVCRT_HOME%" (
        echo ERROR: MSVCRT_HOME "%MSVCRT_HOME%" is invalid.
        goto error
)

if not exist "%UIMA_SCRIPTATORSDIR%"\uima.i (
	echo ERROR: UIMA_SCRIPTATORSDIR "%UIMA_SCRIPTATORSDIR%" is invalid.
	goto error
)

if "%2" == "clean" (
  echo removing %UIMA_DIR%
  rmdir %UIMA_DIR% /s /q
)

if exist %UIMA_DIR% (
	echo ERROR: directory %UIMA_DIR% already exists. Please use "clean" option
	goto error
)

REM Create the top-level directories
mkdir %UIMA_DIR%
mkdir %UIMA_DIR%\bin
mkdir %UIMA_DIR%\lib
mkdir %UIMA_DIR%\data
mkdir %UIMA_DIR%\docs
mkdir %UIMA_DIR%\include
mkdir %UIMA_DIR%\examples
mkdir %UIMA_DIR%\scriptators
REM mkdir %UIMA_DIR%\licenses

echo.
echo copying from %UIMA_INSTALLDIR%...
xcopy /Q /Y %UIMA_INSTALLDIR%\bin\uima.dll %UIMA_DIR%\bin
xcopy /Q /Y %UIMA_INSTALLDIR%\bin\uimaD.dll %UIMA_DIR%\bin
xcopy /Q /Y %UIMA_INSTALLDIR%\bin\runAECpp.exe %UIMA_DIR%\bin
xcopy /Q /Y %UIMA_INSTALLDIR%\bin\runAECppD.exe %UIMA_DIR%\bin
xcopy /Q /Y %UIMA_INSTALLDIR%\bin\deployCppService.exe %UIMA_DIR%\bin
xcopy /Q /Y %UIMA_INSTALLDIR%\bin\deployCppServiceD.exe %UIMA_DIR%\bin
xcopy /Q /Y %UIMA_INSTALLDIR%\data\resourceSpecifierSchema.xsd %UIMA_DIR%\data
xcopy /Q /Y %UIMA_INSTALLDIR%\lib\uima.lib %UIMA_DIR%\lib
xcopy /Q /Y %UIMA_INSTALLDIR%\lib\uimaD.lib %UIMA_DIR%\lib
mkdir %UIMA_DIR%\include\uima
xcopy /Q /Y %UIMA_INSTALLDIR%\include\uima\* %UIMA_DIR%\include\uima

echo.
echo copying from %UIMA_DOCDIR%...
xcopy /Q /Y %UIMA_DOCDIR%\overview_and_setup.html %UIMA_DIR%\docs
xcopy /Q /Y %UIMA_DOCDIR%\uimadoxytags.tag %UIMA_DIR%\docs
mkdir %UIMA_DIR%\docs\html
xcopy /Q /Y %UIMA_DOCDIR%\html\* %UIMA_DIR%\docs\html

echo.
echo copying from %UIMA_EXAMPLESDIR%...
xcopy /Q /Y %UIMA_EXAMPLESDIR%\*.html %UIMA_DIR%\examples
xcopy /Q /Y %UIMA_EXAMPLESDIR%\*.cmd %UIMA_DIR%\examples
mkdir %UIMA_DIR%\examples\data
xcopy /Q /Y %UIMA_EXAMPLESDIR%\data\* %UIMA_DIR%\examples\data
mkdir %UIMA_DIR%\examples\descriptors
xcopy /Q /Y %UIMA_EXAMPLESDIR%\descriptors\*.xml %UIMA_DIR%\examples\descriptors\*.xml
mkdir %UIMA_DIR%\examples\src
xcopy /Q /Y %UIMA_EXAMPLESDIR%\src\*.cpp %UIMA_DIR%\examples\src
xcopy /Q /Y %UIMA_EXAMPLESDIR%\src\*.vcproj %UIMA_DIR%\examples\src
xcopy /Q /Y %UIMA_EXAMPLESDIR%\src\*.sln %UIMA_DIR%\examples\src
REM copy the following file separately, as it is part of the fvt suite
xcopy /Q /Y %UIMA_TESTSRCDIR%\SofaStreamHandlerFile.cpp %UIMA_DIR%\examples\src
xcopy /Q /Y %UIMA_TESTSRCDIR%\SimpleTextSegmenter.cpp %UIMA_DIR%\examples\src

mkdir %UIMA_DIR%\examples\tutorial
mkdir %UIMA_DIR%\examples\tutorial\descriptors
mkdir %UIMA_DIR%\examples\tutorial\src
xcopy /Q /Y %UIMA_EXAMPLESDIR%\tutorial\*.* %UIMA_DIR%\examples\tutorial
xcopy /Q /Y %UIMA_EXAMPLESDIR%\tutorial\descriptors\*.xml %UIMA_DIR%\examples\tutorial\descriptors
xcopy /Q /Y %UIMA_EXAMPLESDIR%\tutorial\src\*.vcproj %UIMA_DIR%\examples\tutorial\src
REM copy the following files from the fvt suite
xcopy /Q /Y %UIMA_TESTSRCDIR%\MeetingAnnotator.cpp %UIMA_DIR%\examples\tutorial\src
xcopy /Q /Y %UIMA_TESTSRCDIR%\..\MeetingAnnotator.dll %UIMA_DIR%\examples\tutorial\src

echo.
echo copying from %APR_HOME%...
mkdir %UIMA_DIR%\include\apr
xcopy /Q /Y %APR_HOME%\include\apr*.h %UIMA_DIR%\include\apr
xcopy /Q /Y %APR_HOME%\Release\libapr*.dll %UIMA_DIR%\bin
xcopy /Q /Y %APR_HOME%\Release\libapr*.lib %UIMA_DIR%\lib


echo.
echo copying from %ICU_HOME%...
mkdir %UIMA_DIR%\include\unicode
xcopy /S /Q /Y %ICU_HOME%\include\unicode %UIMA_DIR%\include\unicode
xcopy /Q /Y %ICU_HOME%\bin\icu*.dll %UIMA_DIR%\bin
xcopy /Q /Y %ICU_HOME%\lib\icu*.lib %UIMA_DIR%\lib

echo.
echo copying from %XERCES_HOME%...
mkdir %UIMA_DIR%\include\xercesc
xcopy /S /Q /Y %XERCES_HOME%\include\xercesc %UIMA_DIR%\include\xercesc
xcopy /Q /Y %XERCES_HOME%\bin\xerces-c_2*.dll %UIMA_DIR%\bin
xcopy /Q /Y %XERCES_HOME%\lib\xerces-c_2*.lib %UIMA_DIR%\lib

echo.
echo copying MSVC redistribution libs
xcopy /q /y "%MSVCRT_HOME%"\msvc*.dll %UIMA_DIR%\bin

echo.
echo copying the scriptators...
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\uima.i %UIMA_DIR%\scriptators
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\ThreadAnnotator.h %UIMA_DIR%\scriptators

echo.
echo copying the Perl scriptator...
mkdir %UIMA_DIR%\scriptators\perl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\perl\Perl.html %UIMA_DIR%\scriptators\perl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\perl\Perl.html %UIMA_DIR%\docs
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\perl\*.cpp %UIMA_DIR%\scriptators\perl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\perl\*.pl  %UIMA_DIR%\scriptators\perl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\perl\*.xml %UIMA_DIR%\scriptators\perl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\perl\*.vcproj %UIMA_DIR%\scriptators\perl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\perl\*.cmd %UIMA_DIR%\scriptators\perl

echo.
echo copying Python scriptator...
mkdir %UIMA_DIR%\scriptators\python
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\python\Python.html %UIMA_DIR%\scriptators\python
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\python\Python.html %UIMA_DIR%\docs
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\python\*.cpp %UIMA_DIR%\scriptators\python
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\python\*.py  %UIMA_DIR%\scriptators\python
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\python\*.xml %UIMA_DIR%\scriptators\python
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\python\*.vcproj %UIMA_DIR%\scriptators\python
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\python\*.cmd %UIMA_DIR%\scriptators\python

echo.
echo copying Tcl scriptator...
mkdir %UIMA_DIR%\scriptators\tcl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\tcl\Tcl.html %UIMA_DIR%\scriptators\tcl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\tcl\Tcl.html %UIMA_DIR%\docs
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\tcl\*.cpp %UIMA_DIR%\scriptators\tcl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\tcl\*.tcl %UIMA_DIR%\scriptators\tcl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\tcl\*.xml %UIMA_DIR%\scriptators\tcl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\tcl\*.vcproj %UIMA_DIR%\scriptators\tcl
xcopy /Q /Y %UIMA_SCRIPTATORSDIR%\tcl\*.cmd %UIMA_DIR%\scriptators\tcl

echo.
echo copying licenses...

REM copy Apache licences
xcopy /Q /Y %UIMA_SOURCE%\NOTICE.4bin %UIMA_DIR%
ren %UIMA_DIR%\NOTICE.4bin NOTICE
xcopy /Q /Y %UIMA_SOURCE%\LICENSE %UIMA_DIR%
xcopy /Q /Y %UIMA_SOURCE%\RELEASE_NOTES.* %UIMA_DIR%
xcopy /Q /Y %UIMA_SOURCE%\README.4bin %UIMA_DIR%
ren %UIMA_DIR%\README.4bin README

REM mkdir %UIMA_DIR%\licenses\apr
REM xcopy /Q /Y %APR_HOME%\LICENSE* %UIMA_DIR%\licenses\apr
REM if not exist %UIMA_DIR%\licenses\icu mkdir %UIMA_DIR%\licenses\icu
REM xcopy /Q /Y %ICU_HOME%\LICENSE* %UIMA_DIR%\licenses\icu
REM if not exist %UIMA_DIR%\licenses\xerces mkdir %UIMA_DIR%\licenses\xerces
REM xcopy /Q /Y %XERCES_HOME%\LICENSE* %UIMA_DIR%\licenses\xerces

if not "%ACTIVEMQ_HOME%" == "" (
  echo.
  echo copying from %APU_HOME%...
  xcopy /Q /Y %APU_HOME%\Release\libaprutil*.dll %UIMA_DIR%\bin
  xcopy /Q /Y %APU_HOME%\Release\libaprutil*.lib %UIMA_DIR%\lib
  echo.
  echo copying from %ACTIVEMQ_HOME%...
  xcopy /Q /Y %ACTIVEMQ_HOME%\vs2008-build\win32\ReleaseDLL\activemq-cpp.dll %UIMA_DIR%\bin
  xcopy /Q /Y %ACTIVEMQ_HOME%\vs2008-build\win32\DebugDLL\activemq-cppd.dll %UIMA_DIR%\bin
)

echo cleaning the target tree
for /R %UIMA_DIR% %%d in (.) do del %%d\*~ 2> NUL

echo DONE SDK image in %UIMA_DIR%
goto end

:Missing
echo APR_HOME and ICU_HOME and XERCES_HOME must all be specified
echo and must contain the directories produced by their "install" builds
goto end

:MissingAprUtil
echo APU_HOME must be specifed when ACTIVEMQ_HOME is specified
echo and must contain the directories produced by apr-util "install" build
goto end

:error
echo FAILED: UIMA C++ SDK was not built.

:end
