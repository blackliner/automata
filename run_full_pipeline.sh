#!/usr/bin/env bash

set -euxo pipefail

./docker/build.py

./docker/docker_run.py bazel clean

./docker/docker_run.py bazel build ...

./docker/docker_run.py bazel test ...
