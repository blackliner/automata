load("@rules_foreign_cc//tools/build_defs:cmake.bzl", "cmake_external")

cmake_external(
    name = "opencv",
    cmake_options = [
        "-GNinja",
        "-DBUILD_LIST=core,highgui,imgcodecs,imgproc",
    ],
    lib_source = "@opencv//:all",
    make_commands = [
        "ninja",
        "ninja install",
    ],
    out_include_dir = "include/opencv4",
    shared_libraries = [
        "libopencv_core.so",
        "libopencv_highgui.so",
        "libopencv_imgcodecs.so",
        "libopencv_imgproc.so",
    ],
    visibility = ["//visibility:public"],
)

cmake_external(
    name = "sfml",
    cmake_options = [
        "-GNinja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DSFML_BUILD_EXAMPLES=OFF",
    ],
    lib_source = "@sfml//:all",
    make_commands = [
        "ninja",
        "ninja install",
    ],
    out_include_dir = "include",
    shared_libraries = [
        # "libsfml-audio.so",
        "libsfml-audio.so.2.5",
        # "libsfml-audio.so.2.5.1",
        # "libsfml-graphics.so",
        "libsfml-graphics.so.2.5",
        # "libsfml-graphics.so.2.5.1",
        # "libsfml-system.so",
        "libsfml-system.so.2.5",
        # "libsfml-system.so.2.5.1",
        # "libsfml-window.so",
        "libsfml-window.so.2.5",
        # "libsfml-window.so.2.5.1",
    ],
    visibility = ["//visibility:public"],
)
