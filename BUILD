load("//:variables.bzl", "BUILD_COPTS")

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
