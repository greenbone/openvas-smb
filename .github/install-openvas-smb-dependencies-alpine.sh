# This script installs openvas-smb-dependencies.
#/bin/sh
set -ex
apk update && apk upgrade && \
  apk add --no-cache build-base \
  cmake \
  doxygen \
  glib-dev \
  gnutls-dev \
  graphviz \
  gvm-libs-dev \
  heimdal-dev \
  libunistring-dev \
  binutils \
  gcc \
  perl-dev \
  popt-dev \
  samurai \
  sqlite-dev \
  xmltoman \
  git

