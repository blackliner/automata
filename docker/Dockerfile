FROM ubuntu:18.04

RUN apt-get update && apt-get -qq install --no-install-recommends \
    g++ \
    curl \
    gnupg \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

### installing bazel

RUN curl https://bazel.build/bazel-release.pub.gpg | apt-key add - \
    && echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list

RUN apt-get update && apt-get -qq install --no-install-recommends \
    bazel \
    && rm -rf /var/lib/apt/lists/*

### since this changes a lot, do it last

RUN apt-get update && apt-get -qq install --no-install-recommends \
    sudo \
    git \
    lcov \
    libgl1-mesa-dev \
    libsfml-dev \
    libpng-dev \
    && rm -rf /var/lib/apt/lists/*


RUN apt-get update && apt-get -qq install --no-install-recommends \
    cmake \
    build-essential \
    && rm -rf /var/lib/apt/lists/*


RUN git clone https://github.com/opencv/opencv.git 
RUN mkdir /opencv/build
WORKDIR /opencv/build
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
RUN make -j 24
RUN make install

WORKDIR /automata

# COPY entry.sh /etc/

# ENTRYPOINT /etc/entry.sh

