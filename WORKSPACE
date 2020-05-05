workspace(
    name = "automata",
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")  #new:git_repository is needed for projects without a BUILD file ;-)

git_repository(
    name = "com_google_benchmark",
    commit = "090faecb454fbd6e6e17a75ef8146acb037118d4",
    remote = "https://github.com/google/benchmark",
    shallow_since = "1557776538 +0300",
)

git_repository(
    name = "com_google_googletest",
    commit = "703bd9caab50b139428cea1aaff9974ebee5742e",
    remote = "https://github.com/google/googletest",
    shallow_since = "1570114335 -0400",
)

git_repository(
    name = "com_github_google_rules_install",
    remote = "https://github.com/google/bazel_rules_install.git",
    tag = "0.0.2",
)

load("@com_github_google_rules_install//:deps.bzl", "install_rules_dependencies")

install_rules_dependencies()

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

http_archive(
    name = "opencv",
    build_file_content = all_content,
    strip_prefix = "opencv-4.3.0",
    urls = ["https://github.com/opencv/opencv/archive/4.3.0.zip"],
)

http_archive(
    name = "rules_foreign_cc",
    strip_prefix = "rules_foreign_cc-master",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

# new_local_repository(
#     name = "usr_local",
#     build_file = "usr_local.BUILD",
#     path = "/usr/local",
# )

# needed packages
# sudo apt install libpng-dev
# sudo apt install libgl1-mesa-dev
