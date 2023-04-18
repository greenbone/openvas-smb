FROM debian:stable-slim
LABEL deprecated="This image is deprecated and may be removed soon."
RUN apt-get update && apt-get install --no-install-recommends --no-install-suggests -y \
  build-essential \
  cmake \
  pkg-config \
  gcc-mingw-w64 \
  libgnutls28-dev \
  perl-base \
  heimdal-dev \
  libpopt-dev \
  libglib2.0-dev \
  libunistring-dev \
  && rm -rf /var/lib/apt/lists/*
