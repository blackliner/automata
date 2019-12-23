load("//:variables.bzl", "BUILD_COPTS", "TEST_COPTS")

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

cc_binary(
    name = "neural",
    srcs = [
        "neural.cpp",
    ],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
)
