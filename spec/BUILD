#cc_test(
#    name = "find_square_sum_test",
#    srcs = ["find_square_sum_test.cpp"],
#    copts = ["-Iexternal/gtest/include",
#             "-Ilib"],
#    deps = [
#        "@gtest_http_archive//:gtest",
#        "//lib:find_square_sum",
#    ],
#    visibility = ["//visibility:public"],
#)

cc_test(
    name = "hashtablespec",
    srcs = ["hashtablespec.cpp"],
    copts = [
        "-Iexternal/gtest/include",
        "-Ilib",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:hashtable",
        "@gtest_http_archive//:gtest",
    ],
)

cc_test(
    name = "quad_tree_spec",
    srcs = ["quad_tree_spec.cpp"],
    copts = [
        "-Iexternal/gtest/include",
        "-Ilib",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:quad_tree",
        "@gtest_http_archive//:gtest",
    ],
)
