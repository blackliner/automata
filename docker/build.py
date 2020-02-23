#!/usr/bin/env python3

import argparse
import os
import subprocess

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Build docker images.')
    parser.add_argument('-n', '--name', default='bazel_build')

    args = parser.parse_args()

    docker_dir = os.path.dirname(os.path.realpath(__file__))

    command = ['docker', 'build', '--network=host',
               '-t', args.name, docker_dir]

    subprocess.call(command)
