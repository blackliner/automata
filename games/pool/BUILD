load("//variables:variables.bzl", "BUILD_COPTS")
load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "pool",
    srcs = [
        "main.cpp",
    ],
    copts = BUILD_COPTS,
    data = ["arial.ttf"],
    deps = ["//:sfml"],
)
