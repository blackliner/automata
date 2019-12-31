load("//:variables.bzl", "BUILD_COPTS", "TEST_COPTS")

cc_test(
    name = "quad_tree_spec",
    srcs = ["quad_tree_spec.cpp"],
    copts = TEST_COPTS + ["-Ilib"],
    visibility = ["//visibility:public"],
    deps = [
        "//quadtree:quad_tree",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "quad_tree",
    srcs = ["quad_tree.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
    deps = ["//lib:vector2d"],
)
