load("//variables:variables.bzl", "BUILD_COPTS")
load("@com_github_google_rules_install//installer:def.bzl", "installer")
load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "automata",
    srcs = [
        "main.cpp",
        "main.h",
    ],
    copts = BUILD_COPTS + ["-Ilib"] + ["-Iquadtree"],
    deps = [
        "//lib:olc_pixel_game_engine",
        "//lib:vehicle",
        "//quadtree",
    ],
)

installer(
    name = "install_automata",
    data = [":automata"],
)
