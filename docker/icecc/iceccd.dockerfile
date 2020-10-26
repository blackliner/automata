FROM ubuntu:18.04 AS base

RUN apt-get update
RUN apt-get install -qq \
    git \
    gcc \
    g++ \
    pkg-config \
    make \
    automake \
    libtool
RUN apt-get install -qq \
    libcap-ng-dev \
    libarchive-dev \
    libzstd-dev \
    liblzo2-dev
RUN rm -rf /var/lib/apt/lists/*

WORKDIR /tmp
RUN git clone https://github.com/icecc/icecream.git
WORKDIR /tmp/icecream
RUN ./autogen.sh
RUN ./configure --prefix=/opt/icecream
RUN make -j
# run make install || true

FROM base as iceccd

ENTRYPOINT ["/icecream/daemon/iceccd","-vvv"]


FROM base as icecc-scheduler

ENTRYPOINT ["/icecream/scheduler/icecc-scheduler","-vvv"]
