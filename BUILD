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
        "//quadtree:quad_tree",
        "//lib:olc_pixel_game_engine",
        #"//lib:vector2d",
    ],
)


installer(
    name = "install",
    data = [":auto"],
)