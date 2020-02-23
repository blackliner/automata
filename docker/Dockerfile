FROM ubuntu:18.04

RUN apt-get update && apt-get -qq install \
    curl \
    gnupg \
    && rm -rf /var/lib/apt/lists/*

### installing bazel

RUN curl https://bazel.build/bazel-release.pub.gpg | apt-key add - \
    && echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list

RUN apt-get update && apt-get -qq install \
    bazel \
    && rm -rf /var/lib/apt/lists/*

### since this changes a lot, do it last

RUN apt-get update && apt-get -qq install \
    sudo \
    git \
    lcov \
    libgl1-mesa-dev \
    libsfml-dev \
    libpng-dev \
    && rm -rf /var/lib/apt/lists/*

COPY entry.sh /etc/

ENTRYPOINT /etc/entry.sh

