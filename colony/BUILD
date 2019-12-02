cc_test(
    name = "colony_spec",
    srcs = ["colony_spec.cpp"],
    copts = [
        "-Iexternal/gtest/include",
        "-Ilib",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//colony:colony",
        "@gtest_http_archive//:gtest",
    ],
)

cc_library(
    name = "colony",
    srcs = ["colony.cpp", "colony.h"],
    visibility = ["//visibility:public"],
)