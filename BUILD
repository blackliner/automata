load("//:variables.bzl", "BUILD_COPTS")


cc_binary(
    name = "auto",
    srcs = [
        "Main.cpp",
        "lib/Main.h",
    ],
    copts = BUILD_COPTS + ["-Ilib"],
    deps = [
        "vehicle",
        "//quadtree:quad_tree",
        "olc_pixel_game_engine",
        #"//lib:vector2d",
    ],
)

cc_library(
    name = "renderer",
    srcs = ["lib/Renderer.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "vehicle",
    srcs = ["Vehicle.cpp"],
    hdrs = ["lib/Vehicle.h"],
    copts = BUILD_COPTS + ["-Ilib"],
    visibility = ["//visibility:public"],
    deps = [
        "path_segment",
        "perlin",
        "renderer",
        "weapons",
    ],
)

cc_library(
    name = "olc_pixel_game_engine",
    hdrs = ["lib/olcPixelGameEngine.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
    deps = [
        #"@system_libs//:all", # would work if there was no libpthread_db.so :-(
        "@system_libs//:pthread",
        "@system_libs//:x11",
        "@system_libs//:png",
        "@system_libs//:gl",
    ],
)

cc_library(
    name = "vector2d",
    hdrs = ["lib/Vector2D.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "weapons",
    srcs = ["Weapons.cpp"],
    hdrs = ["lib/Weapons.h"],
    copts = BUILD_COPTS + ["-Ilib"],
    visibility = ["//visibility:public"],
    deps = ["vector2d"],
)

cc_library(
    name = "path_segment",
    hdrs = ["lib/PathSegment.h"],
    copts = BUILD_COPTS + ["-Ilib"],
    visibility = ["//visibility:public"],
    deps = ["vector2d"],
)

cc_library(
    name = "perlin",
    hdrs = ["lib/Perlin.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "hashtable",
    hdrs = ["lib/hashtable.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
    deps = ["hash"],
)

cc_library(
    name = "hash",
    hdrs = ["lib/hash.h"],
    copts = BUILD_COPTS,
    visibility = ["//visibility:public"],
)
