UIMA C++ Examples
=================

This directory contains example code that illustrate how to use the UIMA
C++ Framework.

Building the Examples
---------------------

Build the examples as follows:

```bash
$ sudo DOCKER_BUILDKIT=1 docker build -t localuser:uimacppex --output lib .
```

This will create the shared libraries in lib.

To run it:

```bash
$ sudo docker run -v $PWD/lib:/usr/local/uimacpp/ae -v $PWD/data:/data -v $PWD/out:/out -v $PWD/descriptors:/descriptors apache:uimacpp /descriptors/DaveDetector.xml /data/example.txt /out
```


Running the Sample UIMA Components
----------------------------------

These components can be run using either in the native C++ environment
using the `runAECpp` program or run from Java using the `runAE` utility
or integrated into a CPE. The UIMA C++ descriptors are located in the
descriptors subdirectory.

### DaveDetector

A UIMA annotator that finds Daves in text and annotates them. It has one
configuration parameter, `DaveString`, that specifies the string to
match. It illustrates how to use the CAS APIs to create annotations and
add them to the index.

To run this annotator in C++:

### SofaExampleAnnotator

A simple multi-Sofa example annotator that expects an English text Sofa
as input and creates a German text Sofa as output. This annotator has no
configuration parameters, and requires no initialization method. To run
this annotator in C++:

### SofaStreamHandlerFile

This component implements the Sofa stream handler interface defined in
`sofastreamhandler.hpp` to provide stream access to data located using
the `file:` URI scheme. It enables a UIMA component to access remote
Sofa data referenced with a `file:` URI. This example may be used as a
model for building handlers for custom URI schemes. The shared library
`SofaStreamHandlerFile` must be registered with the framework as
follows:

On Windows

On Linux

Handlers for several URI schemes may be registered separated by a blank.
There can be only one handler per URI scheme.

The `SofaDataAnnotator` described below illustrates reading Sofa data as
a stream.

### SofaDataAnnotator

An annotator that accesses the data in Sofa \"EnglishDocument\" as a
text stream. It tokenizes the data on whitespace and creates an
annotation for each token. The annotator may be run with an input Sofa
where the Sofa data is local or with a Sofa where the Sofa data is
remote and specified as a URI. To run this annotator in C++ to process a
Sofa with local data:

To run and process Sofa where the Sofa data is specified as a `file:`
URL, register the example `sofaStreamFileHandler` handler for the `file`
URI scheme as described in the section above. and run:

### SimpleTextSegmenter

A simple example CAS Multiplier which is a type of analysis component
that outputs new CASes. This example illustrates one use of the CAS
Multiplier which is to break down a large CAS into smaller pieces which
are put into new CASs. The `SimpleTextSegmenter` breaks down the input
document into segments based on a delimiter and creates a Sofa for each
segment in a new CAS. The delimiter to use can be specified by setting
the value of the configuration parameter `DelimiterString` in the
descriptor. To run this annotator in C++:

The original input CAS as well as the new CASs containing the segments
will be written out as separate files in your \<yourOutputDir\> in XMI
format.

### XCasWriterCasConsumer

A simple example CAS Consumer that generates an XCAS (XML representation
of a CAS) and writes it to stdout by default. It can be configured to
write the XCAS output to a file in a directory specified by modifying
the descriptor and setting a value for the configuration parameter
`OutputDirectory`. The `XCasWriterCasConsumer` can be inserted at any
point in a aggregate or CPE flow to dump the contents of CAS and is
useful for debugging. To run this annotator in C++:

To see the results of the earlier examples:

Running the Sample UIMA Applications
------------------------------------

These illustrate how to write stand-alone C++ applications that run UIMA
C++ components. Build the examples and set up environment variables as
described above.

### ExampleApplication

This application reads all the .txt files in a directory, creates a CAS
for each in turn and sends them through an AnalysisEngine. The results
are printed on stdout as an XCAS. The application takes two arguments,
the path to a UIMA C++ descriptor file, and a file or directory
containing input data:

### SofaExampleApplication

A multiple Sofa example that creates a text Sofa called EnglishDocument
and sets its Sofa data to some English text and calls the
SofaExampleAnnotator which produces a Sofa with German text and writes
the annotations to stdout. This application takes one argument, the path
to the SofaExampleAnnotator descriptor file:
