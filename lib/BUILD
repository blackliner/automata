load("//:variables.bzl", "BUILD_COPTS")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "renderer",
    srcs = ["Renderer.h"],
    copts = BUILD_COPTS,
    deps = ["vector2d"],
)

cc_library(
    name = "matrix2d",
    srcs = ["matrix2d.h"],
    copts = BUILD_COPTS,
)

cc_library(
    name = "vehicle",
    srcs = ["Vehicle.cpp"],
    hdrs = ["Vehicle.h"],
    copts = BUILD_COPTS,
    deps = [
        "path_segment",
        "perlin",
        "renderer",
        "weapons",
    ],
)

cc_library(
    name = "olc_pixel_game_engine",
    hdrs = ["olcPixelGameEngine.h"],
    copts = BUILD_COPTS,
    linkopts = [
        "-lpthread",
        "-lX11",
        "-lpng",
        "-lGL",
    ],
)

cc_library(
    name = "vector2d",
    hdrs = ["Vector2D.h"],
    copts = BUILD_COPTS,
)

cc_library(
    name = "weapons",
    srcs = ["Weapons.cpp"],
    hdrs = ["Weapons.h"],
    copts = BUILD_COPTS,
    deps = ["vector2d"],
)

cc_library(
    name = "path_segment",
    hdrs = ["PathSegment.h"],
    copts = BUILD_COPTS,
    deps = ["vector2d"],
)

cc_library(
    name = "perlin",
    hdrs = ["Perlin.h"],
    copts = BUILD_COPTS,
)

cc_library(
    name = "hashtable",
    hdrs = ["hashtable.h"],
    copts = BUILD_COPTS,
    deps = ["hash"],
)

cc_library(
    name = "hash",
    hdrs = ["hash.h"],
    copts = BUILD_COPTS,
)
