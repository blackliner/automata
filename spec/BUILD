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
    name = "hashtablespec",
    srcs = ["hashtablespec.cpp"],
    copts = TEST_COPTS + [
        "-Ilib",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:hashtable",
        "@com_google_googletest//:gtest",
    ],
)

cc_test(
    name = "quad_tree_spec",
    srcs = ["quad_tree_spec.cpp"],
    copts = TEST_COPTS + [
        "-Ilib",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:quad_tree",
        "@com_google_googletest//:gtest",
    ],
)
