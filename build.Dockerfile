# Build SDK

FROM debian:bookworm-slim AS build
WORKDIR /uima-cpp
RUN apt-get update && apt-get -y install --no-install-suggests libapr1-dev libxerces-c-dev libicu-dev openjdk-17-jdk-headless build-essential pkgconf autoconf file libtool m4 automake swig4.0 libpython3-dev 
COPY . .
RUN ./autogen.sh
RUN mkdir /usr/local/uimacpp
RUN mkdir /usr/local/uimacpp/ae
RUN mkdir /usr/local/uimacpp/desc
RUN mkdir /usr/local/uimacpp/scripts
RUN ./configure --prefix=/usr/local/uimacpp --without-activemq --with-jdk=/usr/lib/jvm/java-17-openjdk-amd64/
RUN make -j4
RUN make install
# run tests
RUN make check

# build pythonator
RUN echo '/usr/local/uimacpp/lib' >> /etc/ld.so.conf.d/uimacpp.conf && ldconfig
WORKDIR /uima-cpp/scriptators
RUN UIMACPP_HOME=/usr/local/uimacpp make
RUN cp libpythonator.so /usr/local/uimacpp/lib/
RUN cp Pythonator.xml /usr/local/uimacpp/desc/
RUN cp ae.py pythonnator.py /usr/local/uimacpp/scripts/

# dev
RUN apt-get install -y --no-install-recommends emacs-nox gdb less
ENTRYPOINT ["/bin/bash"]
