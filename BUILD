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
#
#cc_test(
#    name = "multi_inherit_test",
#    srcs = ["multi_inherit_test.cpp"],
#    copts = ["-Iexternal/gtest/include",
#             "-Ilib"],
#    deps = [
#        "@gtest_http_archive//:gtest",
#        "//lib:multi_inherit",
#    ],
#    visibility = ["//visibility:public"],
#)

cc_binary(
    name = "auto",
    srcs = ["Main.cpp", "lib/Main.h"],
    copts = ["-Ilib"],
    deps = ["vehicle", "olc_pixel_game_engine",
    #"//lib:vector2d",
    ]
)

cc_library(
    name = "vehicle",
    srcs = ["Vehicle.cpp"],
    hdrs = ["lib/Vehicle.h"],
    copts = ["-Ilib"],
    deps = ["path_segment", "perlin", "weapons"],
    visibility = ["//visibility:public"]
)


cc_library(
    name = "olc_pixel_game_engine",
    hdrs = ["lib/olcPixelGameEngine.h"],
    deps = [
        #"@system_libs//:all", # would work if there was no libpthread_db.so :-(
        "@system_libs//:pthread",
        "@system_libs//:x11",
        "@system_libs//:png",
        "@system_libs//:gl",
        ],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "vector2d",
    hdrs = ["lib/Vector2D.h"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "weapons",
    srcs = ["Weapons.cpp"],
    hdrs = ["lib/Weapons.h"],
    copts = ["-Ilib"],
    deps = ["vector2d"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "path_segment",
    hdrs = ["lib/PathSegment.h"],
    copts = ["-Ilib"],
    deps = ["vector2d"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "perlin",
    hdrs = ["lib/Perlin.h"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "hashtable",
    hdrs = ["lib/hashtable.h"],
    deps = ["hash"],    
    visibility = ["//visibility:public"]
)

cc_library(
    name = "hash",
    hdrs = ["lib/hash.h"],
    visibility = ["//visibility:public"]
)
