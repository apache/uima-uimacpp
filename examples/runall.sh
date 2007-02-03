echo .
echo runAECpp descriptors/DaveDetector.xml data/example.txt
runAECpp descriptors/DaveDetector.xml data/example.txt || exit 99
echo .
echo runAECpp descriptors/DaveDetector.xml -x data/tcas.xcas
runAECpp descriptors/DaveDetector.xml -x data/tcas.xcas || exit 99
echo .
echo runAECpp descriptors/DaveDetector.xml -x data/sofa.xcas -s EnglishDocument
runAECpp descriptors/DaveDetector.xml -x data/sofa.xcas -s EnglishDocument || exit 99
echo .
echo runAECpp -x descriptors/SofaExampleAnnotator.xml data/sofa.xcas
runAECpp -x descriptors/SofaExampleAnnotator.xml data/sofa.xcas || exit 99
echo .
echo export UIMACPP_STREAMHANDLERS=file:SofaStreamHandlerFile
export UIMACPP_STREAMHANDLERS=file:SofaStreamHandlerFile || exit 99
echo .
echo runAECpp -x descriptors/SofaDataAnnotator.xml data/sofa.xcas
runAECpp -x descriptors/SofaDataAnnotator.xml data/sofa.xcas || exit 99
echo .
echo runAECpp -x descriptors/SofaDataAnnotator.xml data/filetcas.xcas
runAECpp -x descriptors/SofaDataAnnotator.xml data/filetcas.xcas || exit 99
echo .
echo runAECpp -x descriptors/SimpleTextSegmenter.xml data/docforsegmenter.xcas
runAECpp -x descriptors/SimpleTextSegmenter.xml data/docforsegmenter.xcas || exit 99
echo .
echo runAECpp -x descriptors/XCasWriterCasConsumer.xml data/tcas.xcas
runAECpp -x descriptors/XCasWriterCasConsumer.xml data/tcas.xcas || exit 99
echo .
echo runAECpp -x descriptors/XCasWriterCasConsumer.xml data/sofa.xcas
runAECpp -x descriptors/XCasWriterCasConsumer.xml data/sofa.xcas || exit 99
echo .
echo src/ExampleApplication descriptors/DaveDetector.xml data
src/ExampleApplication descriptors/DaveDetector.xml data || exit 99
echo .
echo src/SofaExampleApplication descriptors/SofaExampleAnnotator.xml
src/SofaExampleApplication descriptors/SofaExampleAnnotator.xml || exit 99
echo .................
echo All examples ran!
echo .................
