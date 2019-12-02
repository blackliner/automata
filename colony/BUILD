TEST_COPTS = [
    "-pedantic",
    "-pedantic-errors",
    "-std=c++14",
    "-Wall",
    "-Wextra",
    "-Wshadow",
    #    "-Wshorten-64-to-32",
    "-Wfloat-equal",
    "-fstrict-aliasing",
]

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

cc_library(
    name = "colony",
    srcs = ["colony.cpp", "colony.h"],
    visibility = ["//visibility:public"],
)