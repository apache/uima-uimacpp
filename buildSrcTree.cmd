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

REM For now, update version in line below
set srcName=uimacpp-2.4.0

REM check args
if "%~1" == "" (
	echo ERROR: target directory for source tree not specified
	echo ------
	echo Usage: buildSrcTree targetDirectory [clean]
	echo        Builds %srcName% source tree for distribution
	echo assumes: current directory is the root of svn extract
	echo expects: target directory to be non-existant
	echo ------
	goto error
)

set TARGET_DIR=%1\%srcName%

if "%2" == "clean" (
  rmdir %TARGET_DIR% /s /q
)

if exist %TARGET_DIR% (
	echo ERROR: directory %TARGET_DIR% already exists
	echo use "clean" option
	goto error
)
echo Target source tree directory is %TARGET_DIR%

set UIMACPP_SOURCE=%CD%
if not exist %UIMACPP_SOURCE%\src (
	echo ERROR: current directory is not root of UIMACPP source tree
	goto error
)

mkdir %TARGET_DIR%
mkdir %TARGET_DIR%\data
mkdir %TARGET_DIR%\docs
mkdir %TARGET_DIR%\src
mkdir %TARGET_DIR%\src\cas
mkdir %TARGET_DIR%\src\cas\uima
mkdir %TARGET_DIR%\src\framework
mkdir %TARGET_DIR%\src\framework\uima
mkdir %TARGET_DIR%\src\jni
mkdir %TARGET_DIR%\src\jni\uima
mkdir %TARGET_DIR%\src\utils
mkdir %TARGET_DIR%\src\test
mkdir %TARGET_DIR%\src\test\data
mkdir %TARGET_DIR%\src\test\data\descriptors
mkdir %TARGET_DIR%\src\test\data\ExampleCas
mkdir %TARGET_DIR%\src\test\data\segmenterinput
mkdir %TARGET_DIR%\src\test\src
mkdir %TARGET_DIR%\src\test\src\uima

mkdir %TARGET_DIR%\examples
mkdir %TARGET_DIR%\examples\data
mkdir %TARGET_DIR%\examples\descriptors
mkdir %TARGET_DIR%\examples\src
mkdir %TARGET_DIR%\examples\run_configuration
mkdir %TARGET_DIR%\examples\tutorial
mkdir %TARGET_DIR%\examples\tutorial\src
mkdir %TARGET_DIR%\examples\tutorial\descriptors
mkdir %TARGET_DIR%\scriptators
mkdir %TARGET_DIR%\scriptators\perl
mkdir %TARGET_DIR%\scriptators\python
mkdir %TARGET_DIR%\scriptators\tcl

echo.
echo copying from %UIMACPP_SOURCE%
xcopy /Q %UIMACPP_SOURCE%\buildsdk.cmd %TARGET_DIR%
xcopy /Q %UIMACPP_SOURCE%\NOTICE %TARGET_DIR%
xcopy /Q %UIMACPP_SOURCE%\LICENSE %TARGET_DIR%
xcopy /Q %UIMACPP_SOURCE%\RELEASE_NOTES.* %TARGET_DIR%
xcopy /Q %UIMACPP_SOURCE%\README.4src %TARGET_DIR%
ren %TARGET_DIR%\README.4src README
xcopy /Q %UIMACPP_SOURCE%\README.4bin %TARGET_DIR%
xcopy /Q %UIMACPP_SOURCE%\NOTICE.4bin %TARGET_DIR%

echo copying from %UIMACPP_SOURCE%\data
xcopy /Q  %UIMACPP_SOURCE%\data\*.xsd %TARGET_DIR%\data

echo copying from %UIMACPP_SOURCE%\docs
xcopy /Q %UIMACPP_SOURCE%\docs\uimacppdocs.mak %TARGET_DIR%\docs
xcopy /Q %UIMACPP_SOURCE%\docs\builddocs.cmd %TARGET_DIR%\docs
xcopy /Q %UIMACPP_SOURCE%\docs\uimacpp.dox %TARGET_DIR%\docs
xcopy /Q %UIMACPP_SOURCE%\docs\doxyheader.html %TARGET_DIR%\docs
xcopy /Q %UIMACPP_SOURCE%\docs\overview_and_setup.html %TARGET_DIR%\docs

echo copying from %UIMACPP_SOURCE%\src
xcopy /Q %UIMACPP_SOURCE%\src\*.cmd %TARGET_DIR%\src
xcopy /Q %UIMACPP_SOURCE%\src\*.sln %TARGET_DIR%\src
xcopy /Q %UIMACPP_SOURCE%\src\*.bat %TARGET_DIR%\src
xcopy /Q %UIMACPP_SOURCE%\src\*.vcproj %TARGET_DIR%\src

echo copying from %UIMACPP_SOURCE%\src\cas
xcopy /Q %UIMACPP_SOURCE%\src\cas\*.cpp %TARGET_DIR%\src\cas
xcopy /Q %UIMACPP_SOURCE%\src\cas\uima\*.hpp %TARGET_DIR%\src\cas\uima
xcopy /Q %UIMACPP_SOURCE%\src\cas\uima\*.inl %TARGET_DIR%\src\cas\uima

echo copying from %UIMACPP_SOURCE%\src\framework
xcopy /Q %UIMACPP_SOURCE%\src\framework\*.cpp %TARGET_DIR%\src\framework
xcopy /Q %UIMACPP_SOURCE%\src\framework\uima\*.hpp %TARGET_DIR%\src\framework\uima
xcopy /Q %UIMACPP_SOURCE%\src\framework\uima\*.h %TARGET_DIR%\src\framework\uima

echo copying from %UIMACPP_SOURCE%\src\jni
xcopy /Q %UIMACPP_SOURCE%\src\jni\*.cpp %TARGET_DIR%\src\jni
xcopy /Q %UIMACPP_SOURCE%\src\jni\uima\*.hpp %TARGET_DIR%\src\jni\uima
xcopy /Q %UIMACPP_SOURCE%\src\jni\uima\*.h %TARGET_DIR%\src\jni\uima

echo copying from %UIMACPP_SOURCE%\src\test
xcopy /Q %UIMACPP_SOURCE%\src\test\*.sln %TARGET_DIR%\src\test
xcopy /Q %UIMACPP_SOURCE%\src\test\*.bat %TARGET_DIR%\src\test
xcopy /Q %UIMACPP_SOURCE%\src\test\fvtTestfile %TARGET_DIR%\src\test
xcopy /Q %UIMACPP_SOURCE%\src\test\fvtTestfile.debug %TARGET_DIR%\src\test
xcopy /Q %UIMACPP_SOURCE%\src\test\data\*.* %TARGET_DIR%\src\test\data
xcopy /Q %UIMACPP_SOURCE%\src\test\data\segmenterinput\*.* %TARGET_DIR%\src\test\data\segmenterinput
xcopy /Q %UIMACPP_SOURCE%\src\test\data\ExampleCas\*.* %TARGET_DIR%\src\test\data\ExampleCas
xcopy /Q %UIMACPP_SOURCE%\src\test\data\descriptors\*.* %TARGET_DIR%\src\test\data\descriptors
xcopy /Q %UIMACPP_SOURCE%\src\test\src\*.cpp %TARGET_DIR%\src\test\src
xcopy /Q %UIMACPP_SOURCE%\src\test\src\*.vcproj %TARGET_DIR%\src\test\src
xcopy /Q %UIMACPP_SOURCE%\src\test\src\uima\*.hpp %TARGET_DIR%\src\test\src\uima

echo copying from %UIMACPP_SOURCE%\src\utils
xcopy /Q %UIMACPP_SOURCE%\src\utils\*.cpp %TARGET_DIR%\src\utils
xcopy /Q %UIMACPP_SOURCE%\src\utils\*.hpp %TARGET_DIR%\src\utils

echo copying from %UIMACPP_SOURCE%\examples
xcopy /Q %UIMACPP_SOURCE%\examples\run_configuration %TARGET_DIR%\examples\run_configuration
xcopy /Q %UIMACPP_SOURCE%\examples\.project %TARGET_DIR%\examples
xcopy /Q %UIMACPP_SOURCE%\examples\.cdtproject %TARGET_DIR%\examples
xcopy /Q %UIMACPP_SOURCE%\examples\*.cmd %TARGET_DIR%\examples
xcopy /Q %UIMACPP_SOURCE%\examples\*.html %TARGET_DIR%\examples
xcopy /Q %UIMACPP_SOURCE%\examples\data\*.* %TARGET_DIR%\examples\data
xcopy /Q %UIMACPP_SOURCE%\examples\descriptors\*.xml %TARGET_DIR%\examples\descriptors
xcopy /Q %UIMACPP_SOURCE%\examples\src\*.cpp %TARGET_DIR%\examples\src
xcopy /Q %UIMACPP_SOURCE%\examples\src\*.sln %TARGET_DIR%\examples\src
xcopy /Q %UIMACPP_SOURCE%\examples\src\*.vcproj %TARGET_DIR%\examples\src
xcopy /Q %UIMACPP_SOURCE%\examples\tutorial\*.* %TARGET_DIR%\examples\tutorial
xcopy /Q %UIMACPP_SOURCE%\examples\tutorial\descriptors\*.xml %TARGET_DIR%\examples\tutorial\descriptors
xcopy /Q %UIMACPP_SOURCE%\examples\tutorial\src\*.vcproj %TARGET_DIR%\examples\tutorial\src

echo copying from %UIMACPP_SOURCE%\scriptators
xcopy /Q %UIMACPP_SOURCE%\scriptators\*.* %TARGET_DIR%\scriptators
xcopy /Q %UIMACPP_SOURCE%\scriptators\perl\*.* %TARGET_DIR%\scriptators\perl
del %TARGET_DIR%\scriptators\perl\Makefile
xcopy /Q %UIMACPP_SOURCE%\scriptators\python\*.* %TARGET_DIR%\scriptators\python
del %TARGET_DIR%\scriptators\python\Makefile
xcopy /Q %UIMACPP_SOURCE%\scriptators\tcl\*.* %TARGET_DIR%\scriptators\tcl
del %TARGET_DIR%\scriptators\tcl\Makefile

echo cleaning the target tree
for /R %TARGET_DIR% %%d in (.) do del %%d\*~ 2> NUL

echo DONE ... Source image created in %TARGET_DIR%
goto end

:error
	echo FAILED: UIMA C++ source tree was not built.

:end
