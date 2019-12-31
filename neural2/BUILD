load("//:variables.bzl", "BUILD_COPTS", "TEST_COPTS")

cc_test(
    name = "neural2_spec",
    srcs = ["neural2_spec.cpp"],
    copts = TEST_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//neural2",
        "@com_google_googletest//:gtest",
    ],
)

# cc_benchmark(
cc_test(
    name = "neural2_benchmark",
    srcs = ["neural2_benchmark.cpp"],
    copts = TEST_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//neural2",
        "@com_google_benchmark//:benchmark",
    ],
)

cc_library(
    name = "neural2",
    srcs = ["neural2.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
    deps = ["//lib:matrix2d"],
)

cc_binary(
    name = "neural2_main",
    srcs = [
        "neural2_main.cpp",
    ],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//neural2",
    ],
)
