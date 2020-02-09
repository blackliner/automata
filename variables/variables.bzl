TEST_COPTS = [
    "-pedantic",
    "-pedantic-errors",
    "-std=c++17",
    "-Wall",
    "-Wextra",
    "-Wshadow",
    "-Wfloat-equal",
    "-fstrict-aliasing",
    #"-fms-extensions",  # to enable OLC's anonymous struct within the Pixel struct... and it does not work...
]

BUILD_COPTS = TEST_COPTS
