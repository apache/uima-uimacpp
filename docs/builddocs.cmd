@ECHO OFF

@REM  Builds the UIMACPP doxygen docs.
@REM	Requires Doxygen 1.3.6 and Graphviz 1.8.10 installed and 
@REM  PATH environment variable must include
@REM	doxygen\bin;graphviz\bin;graphviz\bin\tools
@REM  This script must be run from the uimacpp\docs subdirectory

@if "%~1"=="" goto build
@if "%~1"=="clean" goto clean
@if "%~1"=="rebuild" goto rebuild

:build
	@echo building uimacpp docs in ..\docs
	nmake -f uimacppdocs.mak build DEL=DEL RD="RM /s /q" MDFILES=..\docs\html\*.md5  MAPFILES=..\docs\html\*.map DOCDIR=..\docs
	goto TheEnd

:rebuild
	@echo rebuilding uimacpp docs in ..\docs
	nmake -f uimacppdocs.mak rebuild DEL=DEL RD="RM /s /q" MDFILES=..\docs\html\*.md5  MAPFILES=..\docs\html\*.map DOCDIR=..\docs
	goto TheEnd

:clean
	@echo cleaning uimacpp docs in ..\docs
	nmake -f uimacppdocs.mak clean DEL=DEL RD="RM /s /q" DOCDIR=..\docs
	goto TheEnd

:TheEnd
	@echo done 

