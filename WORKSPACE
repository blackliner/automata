workspace(
    name = "automata",
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")  #new:git_repository is needed for projects without a BUILD file ;-)

git_repository(
    name = "com_google_benchmark",
    branch = "master",
    remote = "https://github.com/google/benchmark",
    # commit = "367119482ff4abc3d73e4a109b410090fc281337",
    # shallow_since = "1575278956 +0000",
)

git_repository(
    name = "com_google_googletest",
    branch = "master",
    remote = "https://github.com/google/googletest",
    # commit = "367119482ff4abc3d73e4a109b410090fc281337",
    # shallow_since = "1575278956 +0000",
)

# needed packages
# sudo apt install mesa-common-dev
# sudo apt install libpng-dev
# sudo apt install libpthread-stubs0-dev
# sudo apt install libgl1-mesa-dev

new_local_repository(
    name = "system_libs",
    build_file_content = """
cc_library(
    name = "all",
    srcs = glob(["*.so"]),
    visibility = ["//visibility:public"],
)
cc_library(
    name = "x11",
    srcs = ["libX11.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "png",
    srcs = ["libpng.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "pthread",
    srcs = ["libpthread.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "gl",
    srcs = ["libGL.so"],
    visibility = ["//visibility:public"],
)
""",
    # pkg-config --variable=libdir x11
    path = "/usr/lib/x86_64-linux-gnu",
)
