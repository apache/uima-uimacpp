runAECpp descriptors/DaveDetector.xml data/example.txt
@if errorlevel 1 goto error
runAECpp descriptors/DaveDetector.xml -x data/tcas.xcas
@if errorlevel 1 goto error
runAECpp descriptors/DaveDetector.xml -x data/sofa.xcas -s EnglishDocument
@if errorlevel 1 goto error
runAECpp -x descriptors/SofaExampleAnnotator.xml data/sofa.xcas
@if errorlevel 1 goto error
set UIMACPP_STREAMHANDLERS=file:SofaStreamHandlerFile
runAECpp -x descriptors/SofaDataAnnotator.xml data/sofa.xcas
@if errorlevel 1 goto error
runAECpp -x descriptors/SofaDataAnnotator.xml data/filetcas.xcas
@if errorlevel 1 goto error
runAECpp -x descriptors/SimpleTextSegmenter.xml data/docforsegmenter.xcas
@if errorlevel 1 goto error
runAECpp -x descriptors/XCasWriterCasConsumer.xml data/tcas.xcas
@if errorlevel 1 goto error
runAECpp -x descriptors/XCasWriterCasConsumer.xml data/sofa.xcas
@if errorlevel 1 goto error
src\ExampleApplication descriptors/DaveDetector.xml data
@if errorlevel 1 goto error
src\SofaExampleApplication descriptors/SofaExampleAnnotator.xml
@if errorlevel 1 goto error
echo .................
echo All examples ran!
echo .................
:error
