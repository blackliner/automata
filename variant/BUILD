load("//variables:variables.bzl", "TEST_COPTS")
load("//variables:variables.bzl", "BUILD_COPTS")

cc_test(
    name = "variant_spec",
    srcs = ["variant_spec.cpp"],
    copts = TEST_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//variant",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "variant",
    srcs = [
        "variant.cpp",
        "variant.h",
    ],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
)
