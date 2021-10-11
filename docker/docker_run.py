#!/usr/bin/env python3

import argparse
import os
import subprocess

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Build docker images.")
    parser.add_argument("-n", "--name", default="bazel_build")

    args, unknown_args = parser.parse_known_args()

    docker_dir = os.path.dirname(os.path.realpath(__file__))

    user_args = ["--user", f"{os.geteuid()}:{os.getegid()}"]

    volume_args = [
        "--volume",
        "/etc/group:/etc/group:ro",
        "--volume",
        "/etc/passwd:/etc/passwd:ro",
        "--volume",
        "/etc/shadow:/etc/shadow:ro",
        "--volume",
        "/etc/sudoers.d:/etc/sudoers.d:ro",
        "--volume",
        "/var/run/docker.sock:/var/run/docker.sock:rw",
        "--volume",
        "/tmp:/tmp:rw",
        "--volume",
        f'{os.path.expanduser("~")}:{os.path.expanduser("~")}:rw',
        "--volume",
        docker_dir + "/..:/automata:rw",
    ]

    command = (
        ["docker", "run", "--network", "host", "--rm", "--workdir", "/automata"]
        + user_args
        + volume_args
        + [args.name]
        + unknown_args
    )

    retcode = subprocess.call(command)

    exit(retcode)
