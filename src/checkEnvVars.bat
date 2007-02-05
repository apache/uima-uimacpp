@ECHO OFF
@REM Called as a pre-build step to check some environment variables
@REM UIMACPP_HOME is used as the default location of the apr & icu & xerces builds
@REM which are include in the SDK
@REM These may be overridden by setting APR_HOME or ICU_HOME or XERCES_HOME

if "%JAVA_INCLUDE%"=="" (
 if "%JAVA_HOME%"=="" (
   echo Error: JAVA_INCLUDE not defined
   goto end
  )
 set JAVA_INCLUDE=%JAVA_HOME%\include
)

if not exist "%JAVA_INCLUDE%\jni.h" (
  echo Error: failed to find JDK headers in %JAVA_INCLUDE%
  goto end
)

if "%UIMACPP_HOME%"=="" goto noTAF
if "%APR_HOME%"=="" echo Note: Using APR build under UIMACPP_HOME
if "%ICU_HOME%"=="" echo Note: Using ICU build under UIMACPP_HOME
if "%XERCES_HOME%"=="" echo Note: Using XERCES build under UIMACPP_HOME
goto end

:noTAF
if "%APR_HOME%"=="" (
  echo Error: Must set APR_HOME [or use SDK via UIMACPP_HOME]
) else (
  if "%ICU_HOME%"=="" (
    echo Error: Must set ICU_HOME [or use SDK via UIMACPP_HOME]
  ) else (
    if "%XERCES_HOME%"=="" (
      echo Error: Must set XERCES_HOME [or use SDK via UIMACPP_HOME]
    ) else ( goto end ) 
  )
)
echo **** Pre-build check failed ***
exit 99
:end
