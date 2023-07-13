# Build SDK

FROM debian:bookworm-slim AS build
WORKDIR /uima-cpp
RUN apt-get update && apt-get -y install --no-install-suggests libapr1-dev libxerces-c-dev libicu-dev openjdk-17-jdk-headless build-essential pkgconf autoconf file libtool m4 automake swig4.0 libpython3-dev libperl-dev
COPY . .
RUN ./autogen.sh
RUN mkdir /usr/local/uimacpp
RUN mkdir /usr/local/uimacpp/desc
RUN mkdir /usr/local/uimacpp/scripts
RUN mkdir /usr/local/uimacpp/ae
RUN ./configure --prefix=/usr/local/uimacpp --without-activemq --with-jdk=/usr/lib/jvm/java-17-openjdk-amd64/
RUN make -j4
RUN make install
# run tests
RUN make check

# pythonnator
RUN echo '/usr/local/uimacpp/lib' >> /etc/ld.so.conf.d/uimacpp.conf && ldconfig
WORKDIR /uima-cpp/scriptators/python
RUN UIMACPP_HOME=/usr/local/uimacpp make
RUN cp _pythonnator.so /usr/local/uimacpp/lib; cd /usr/local/uimacpp/scripts; ln -s ../lib/_pythonnator.so .
RUN cd /usr/local/uimacpp/lib; ln -s ../scripts/_pythonnator.so ./libpythonnator.so
RUN cp Pythonnator.xml /usr/local/uimacpp/desc/
RUN cp ae.py pythonnator.py /usr/local/uimacpp/scripts/

# perltator
WORKDIR /uima-cpp/scriptators/perl
RUN UIMACPP_HOME=/usr/local/uimacpp make
RUN cp perltator.so /usr/local/uimacpp/lib; cd /usr/local/uimacpp/lib; ln -s perltator.so libperltator.so
RUN cp Perltator.xml /usr/local/uimacpp/desc/
RUN cp ae.pl perltator.pm /usr/local/uimacpp/scripts/

# Generate image with installed files
FROM debian:bookworm-slim
WORKDIR /
COPY --from=build /usr/local/uimacpp /usr/local/uimacpp
RUN apt-get update && apt-get -y install --no-install-suggests libapr1 libxerces-c3.2 libicu72 libpython3.11 libperl5.36 && echo '/usr/local/uimacpp/lib' >> /etc/ld.so.conf.d/uimacpp.conf && ldconfig
COPY docker-entrypoint.sh /
ENTRYPOINT ["/docker-entrypoint.sh"]
