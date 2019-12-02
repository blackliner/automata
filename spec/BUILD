load("//:variables.bzl", "TEST_COPTS")

cc_test(
    name = "hashtablespec",
    srcs = ["hashtablespec.cpp"],
    copts = TEST_COPTS + ["-Ilib"],
    visibility = ["//visibility:public"],
    deps = [
        "//:hashtable",
        "@com_google_googletest//:gtest",
    ],
)

cc_test(
    name = "quad_tree_spec",
    srcs = ["quad_tree_spec.cpp"],
    copts = TEST_COPTS + ["-Ilib"],
    visibility = ["//visibility:public"],
    deps = [
        "//:quad_tree",
        "@com_google_googletest//:gtest",
    ],
)
