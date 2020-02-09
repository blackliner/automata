load("//variables:variables.bzl", "BUILD_COPTS", "TEST_COPTS")

cc_test(
    name = "quadtree_spec",
    srcs = ["quadtree_spec.cpp"],
    copts = TEST_COPTS + ["-Ilib"],
    visibility = ["//visibility:public"],
    deps = [
        "//quadtree",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "quadtree",
    srcs = ["quadtree.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
    deps = ["//lib:vector2d"],
)
