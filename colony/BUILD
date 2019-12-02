load("//:variables.bzl", "TEST_COPTS", "BUILD_COPTS")

cc_test(
    name = "colony_spec",
    srcs = ["colony_spec.cpp"],
    copts = TEST_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//colony:colony",
        "@com_google_googletest//:gtest",
    ],
)


# cc_benchmark(
cc_test(
    name = "colony_benchmark",
    srcs = ["colony_benchmark.cpp"],
    copts = TEST_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//colony:colony",
        "@com_google_benchmark//:benchmark",
    ],
)

cc_library(
    name = "colony",
    srcs = ["colony.cpp", "colony.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
)