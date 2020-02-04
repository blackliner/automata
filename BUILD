load("//:variables.bzl", "BUILD_COPTS")
load("@com_github_google_rules_install//installer:def.bzl", "installer")


package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "auto",
    srcs = [
        "Main.cpp",
        "Main.h",
    ],
    copts = BUILD_COPTS + ["-Ilib"] + ["-Iquadtree"],
    deps = [
        "//lib:vehicle",
        "//quadtree:quadtree",
        "//lib:olc_pixel_game_engine",
    ],
)


installer(
    name = "install",
    # data = ["quadtree"],
    data = ["//quadtree:quadtree"],
)