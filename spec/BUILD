load("//variables:variables.bzl", "TEST_COPTS")

cc_test(
    name = "hashtablespec",
    srcs = ["hashtablespec.cpp"],
    copts = TEST_COPTS + ["-Ilib"],
    visibility = ["//visibility:public"],
    deps = [
        "//lib:hashtable",
        "@com_google_googletest//:gtest",
    ],
)
