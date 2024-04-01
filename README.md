Apache UIMA C++ SDK
===================

The UIMA C++ framework is currently undergoing a number of enhancements to allow for full standalone pipelines written in C++ or in supported scripting languages. As such, there is no available distribution and there are some [major enhancements](https://github.com/apache/uima-uimacpp/issues/6) being [worked on](https://cwiki.apache.org/confluence/display/COMDEV/GSoC+2024+Ideas+list#GSoC2024Ideaslist-UIMA). If interested in contributing, contact the [current maintainer](https://github.com/DrDub).


What is the UIMA C++ SDK?
-------------------------

The UIMA C++ framework is designed to facilitate the creation of UIMA compliant Analysis Engines (AE) from analytics written in C++, or written in languages that can utilize C++ libraries. The UIMACPP SDK directly supports C++, and indirectly supports Perl and Python languages via SWIG (https://www.swig.org/). Existing analytic programs in any of these languages can be wrapped with a UIMACPP annotator and integrated with other UIMA compliant analytics or UIMA-based applications. 

![Framework Core](docs/images/framework-core.png)

The UIMA C++ framework supports testing and embedding UIMA components into native processes. A UIMA C++ test driver, `runAECpp`, is available so that UIMA C++ components can be fully developed and tested in the native environment, no use of Java is needed.

UIMA C++ includes APIs to parse component descriptors, instantiate and call analysis engines, so that UIMA C++ compliant AE can be used in native applications. The Apache UIMA C++ SDK is Docker-based. For interoperability, UIMA C++ components are expected to be built and distributed against a particular Docker image, thus ensuring correct compiler and dependent library settings.


Building
--------

### Checking out the code

Checkout the source code as follows:

    git clone https://github.com/apache/uima-uimacpp.git

UIMACPP runtime prerequisites are APR, ICU, Xerces-C, APR-Util and a JDK for building the JNI interface. 
The SDK also requires doxygen for building the documentation. See the [Dockerfile](Dockerfile) for details.


### Building the Docker image

The Docker image is built on top of Debian stable slim image. After cloning the project, on the root directory do:

```bash
sudo docker build . -t apache:uimacpp
```
This should create an image about 250+ Mb in size.

### Testing the Docker image

The easier way to test it is by running the Perltorator:

```bash
mkdir out
sudo docker run --interactive --tty --name uimacppdev \
  --mount type=bind,source="$(pwd)"/examples/data,target=/data \
  --mount type=bind,source="$(pwd)"/out,target=/out \
  apache:uimacpp \
  /usr/local/uimacpp/desc/Perltator.xml /data /out
```

The `out` folder will be populated by XMI files with the same name as the original files in `data`.

Other useful Docker commands:

```bash
sudo docker rm uimacppdev
```

To remove an old container.

```bash
sudo docker run --interactive --tty --name uimacppdev  --entrypoint /bin/bash apache:uimacpp
```

To run a container interactively using `bash`.



Examples
--------

The UIMACPP package includes several sample UIMA C++ annotators and a sample C++ application that instantiates and uses a C++ annotator. More details on how to build and run the examples will be available over time.

A UIMA C++ annotator descriptor differs from a Java descriptor in the frameworkImplementation, specifying

    <frameworkImplementation>org.apache.uima.cpp</frameworkImplementation>

For a C++ annotator, the annotatorImplementationName specifies the name of a dynamic link library. UIMACPP will add the OS appropriate suffix and search the active dynamic libary path (`LD_LIBRARY_PATH` for Linux). The suffix is not automatically added when the annotatorImplementationName includes a path.
An annotator library is derived from the UIMACPP class "Annotator" and must implement basic annotator methods. Annotators in Perl and Python languages each use a C++ annotator to instantiate the appropriate interpreter, load the specified annotator source and call the annotator methods.



UIMACPP Example - Running a C++ analytic in a Native Process
------------------------------------------------------------

As in UIMA, UIMACPP includes application level methods to instantiate an Analysis Engine from a UIMA annotator descriptor, create a CAS using the AE type system, and call AE methods.

`examples/src/ExampleApplication.cpp` is a simple program that instantiates the specified annotator, reads a directory of txt files, and for each file sets the document text in a CAS and calls the AE process method. For annotator development, this program can be modified to create arbitrary CAS content to drive the annotator. Because the entire application is C++, standard tools such as `gdb` or `devenv` can be easily used for debugging.

`runAECpp` is a UIMA C++ application driver modeled closely after the Java tool runAE. Like `ExampleApplication`, this tool can read a directory of text files and exercise the given annotator. In addition, `runAECpp` can take input from XML format CAS files, call the annotator's `process()` method, and output the resultant CAS in XML format files. XML format CAS input files can be created from upstream UIMA components, or created manually with the content needed to develop and unit test an annotator. This is the default [entrypoint point](docker-entrypoint.sh) for the Docker image.

![UIMA CPP Native Deployment](docs/images/uimacppnative.png)



