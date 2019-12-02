TEST_COPTS = [
    "-pedantic",
    "-pedantic-errors",
    "-std=c++17",
    "-Wall",
    "-Wextra",
    "-Wshadow",
    #    "-Wshorten-64-to-32",
    "-Wfloat-equal",
    "-fstrict-aliasing",
]

cc_test(
    name = "variant_spec",
    srcs = ["variant_spec.cpp"],
    copts = TEST_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//variant:variant",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "variant",
    srcs = ["variant.cpp", "variant.h"],
    visibility = ["//visibility:public"],
)