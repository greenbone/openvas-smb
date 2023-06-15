ARG VERSION=latest

FROM debian:stable-slim AS build
COPY . /source
RUN sh /source/.github/install-openvas-smb-dependencies.sh

RUN cmake -DCMAKE_BUILD_TYPE=Release -B/build /source
RUN DESTDIR=/install cmake --build /build -- install

FROM debian:stable-slim

RUN apt-get update && apt-get install --no-install-recommends --no-install-suggests -y \
  libgnutls30 \
  libgssapi3-heimdal \
  libkrb5-26-heimdal \
  libasn1-8-heimdal \
  libroken19-heimdal \
  libhdb9-heimdal \
  libpopt0 \
  && rm -rf /var/lib/apt/lists/*

COPY --from=build /install/ /

RUN ldconfig
