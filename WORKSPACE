load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "gtest_http_archive",
    url = "https://github.com/google/googletest/archive/release-1.8.0.zip",
    sha256 = "f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
    build_file = "gtest.BUILD",
    strip_prefix = "googletest-release-1.8.0",
)

# needed packages
# sudo apt install mesa-common-dev
# sudo apt install libpng-dev
# sudo apt install libpthread-stubs0-dev
# sudo apt install libgl1-mesa-dev

new_local_repository(
    name = "system_libs",
    # pkg-config --variable=libdir x11
    path = "/usr/lib/x86_64-linux-gnu",
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
)