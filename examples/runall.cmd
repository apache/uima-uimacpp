@setlocal
@set runae=runAECpp
@set exampapp=ExampleApplication
@set sofaexampapp=SofaExampleApplication
@if "%~1" == "debug" (
  @set runae=runAECppD
  @set exampapp=ExampleApplicationD
  @set sofaexampapp=SofaExampleApplicationD
)
%runae% descriptors/DaveDetector.xml data/example.txt
@if errorlevel 1 goto error
%runae% descriptors/DaveDetector.xml -x data/tcas.xcas
@if errorlevel 1 goto error
%runae% descriptors/DaveDetector.xml -x data/sofa.xcas -s EnglishDocument
@if errorlevel 1 goto error
%runae% -x descriptors/SofaExampleAnnotator.xml data/sofa.xcas
@if errorlevel 1 goto error
set UIMACPP_STREAMHANDLERS=file:SofaStreamHandlerFile
%runae% -x descriptors/SofaDataAnnotator.xml data/sofa.xcas
@if errorlevel 1 goto error
%runae% -x descriptors/SofaDataAnnotator.xml data/filetcas.xcas
@if errorlevel 1 goto error
%runae% -x descriptors/SimpleTextSegmenter.xml data/docforsegmenter.xcas
@if errorlevel 1 goto error
%runae% -x descriptors/XCasWriterCasConsumer.xml data/tcas.xcas
@if errorlevel 1 goto error
%runae% -x descriptors/XCasWriterCasConsumer.xml data/sofa.xcas
@if errorlevel 1 goto error
%exampapp% descriptors/DaveDetector.xml data
@if errorlevel 1 goto error
%sofaexampapp% descriptors/SofaExampleAnnotator.xml
@if errorlevel 1 goto error
echo .................
echo All examples ran!
echo .................
:error
