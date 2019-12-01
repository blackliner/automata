cc_test(
    name = "variant_spec",
    srcs = ["variant_spec.cpp"],
    copts = [
        "-Iexternal/gtest/include",
        "-Ilib",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//variant:variant",
        "@gtest_http_archive//:gtest",
    ],
)

cc_library(
    name = "variant",
    srcs = ["variant.cpp", "variant.h"],
    visibility = ["//visibility:public"],
)