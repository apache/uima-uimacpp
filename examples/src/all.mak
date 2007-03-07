#####################################
# Makefile that makes all the eamples
#####################################

EXAMPLES = DaveDetector SofaExampleAnnotator SofaStreamHandlerFile SofaDataAnnotator XCasWriterCasConsumer ExampleApplication SofaExampleApplication SimpleTextSegmenter

all: $(EXAMPLES)

clean:
	rm -f *.o *.so *.dylib

$(EXAMPLES):
	$(MAKE) -f $@.mak
