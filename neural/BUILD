load("//variables:variables.bzl", "BUILD_COPTS", "TEST_COPTS")

cc_test(
    name = "neural_spec",
    srcs = ["neural_spec.cpp"],
    copts = TEST_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//neural",
        "@com_google_googletest//:gtest",
    ],
)

# cc_benchmark(
cc_test(
    name = "neural_benchmark",
    srcs = ["neural_benchmark.cpp"],
    copts = TEST_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//neural",
        "@com_google_benchmark//:benchmark",
    ],
)

cc_library(
    name = "neural",
    srcs = ["neural.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "neural_old",
    srcs = [
        "neural_old.cpp",
    ],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "neural_main",
    srcs = [
        "neural_main.cpp",
    ],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//neural",
    ],
)
