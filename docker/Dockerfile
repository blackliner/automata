FROM ubuntu:18.04

RUN apt-get -qq update

### installing common tools

RUN apt-get -qq install \
    curl \
    gnupg \
    > /dev/null

### installing bazel

RUN curl https://bazel.build/bazel-release.pub.gpg | apt-key add -

RUN echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list

RUN apt-get -qq update

RUN apt-get -qq install bazel > /dev/null



### since this changes a lot, do it last

RUN apt-get -qq install \
    sudo \
    git \
    lcov \
    libgl1-mesa-dev \
    libsfml-dev \
    libpng-dev \
    > /dev/null

COPY entry.sh /etc/

ENTRYPOINT /etc/entry.sh

