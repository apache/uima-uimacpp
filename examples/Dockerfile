FROM apache:uimacpp AS build
WORKDIR /examples
RUN apt-get update && apt-get -y install --no-install-suggests libapr1-dev libxerces-c-dev libicu-dev build-essential pkgconf autoconf
COPY . .
RUN UIMACPP_HOME=/usr/local/uimacpp make -C src -f all.mak

# copy the annotators
FROM scratch
COPY --from=build /examples/src/*.so .
