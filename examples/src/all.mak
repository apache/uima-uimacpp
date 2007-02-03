#####################################
# Makefile that makes all the eamples
#####################################

EXAMPLES = DaveDetector SofaExampleAnnotator SofaStreamHandlerFile SofaDataAnnotator XCasWriterCasConsumer ExampleApplication SofaExampleApplication SimpleTextSegmenter

all: $(EXAMPLES)

$(EXAMPLES):
	$(MAKE) -f $@.mak
