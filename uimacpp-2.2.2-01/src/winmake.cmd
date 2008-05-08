@echo off

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

setlocal

REM default behavior is to build against uima-cpp SDK image, specified by UIMACPP_HOME
REM alternate behavior is to override any or all of the dependent libraries

REM need a Java JRE for the JNI interface
if not exist %JAVA_INCLUDE%\jni.h goto nojava

REM If UIMACPP_HOME undefined, all dependencies must be defined
if not "%UIMACPP_HOME%"=="" (
  REM default the overrides if not set
  if "%APR_HOME%"==""     set APR_HOME=%UIMACPP_HOME%/noPlace
  if "%ICU_HOME%"==""     set ICU_HOME=%UIMACPP_HOME%/noPlace
  if "%XERCES_HOME%"==""  set XERCES_HOME=%UIMACPP_HOME%/noPlace
)else (
  REM UIMACPP_HOME undefined, all dependencies must be defined
  if "%APR_HOME%"==""     goto noapr
  if "%ICU_HOME%"==""     goto noicu
  if "%XERCES_HOME%"==""  goto noxerces
)

REM Start project build
if "%~1" == "" (
 set OPERATION=/build release
) else (
 set OPERATION=%1 %2 %3 %4 %5
)
echo devenv uimacpp.sln %OPERATION%
devenv uimacpp.sln %OPERATION%
goto end

:noapr
echo need UIMACPP_HOME or APR_HOME to be set
goto end

:noicu
echo need UIMACPP_HOME or ICU_HOME to be set
goto end

:noxerces
echo need UIMACPP_HOME or XERCES_HOME to be set
goto end

:nojava
echo JAVA_INCLUDE is not set or does not contain jni.h

:end
endlocal
