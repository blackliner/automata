workspace(
    name = "automata",
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")  #new:git_repository is needed for projects without a BUILD file ;-)

git_repository(
    name = "com_google_benchmark",
    branch = "master",
    remote = "https://github.com/google/benchmark",
)

git_repository(
    name = "com_google_googletest",
    branch = "master",
    remote = "https://github.com/google/googletest",
)

git_repository(
    name = "com_github_google_rules_install",
    branch = "master",
    remote = "https://github.com/google/bazel_rules_install",
)

load("@com_github_google_rules_install//:deps.bzl", "install_rules_dependencies")

install_rules_dependencies()

load("@com_github_google_rules_install//:setup.bzl", "install_rules_setup")

install_rules_setup()

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

new_git_repository(
    name = "opencv",
    branch = "master",
    build_file_content = all_content,
    remote = "https://github.com/opencv/opencv",
)

new_git_repository(
    name = "sfml",
    branch = "master",
    build_file_content = all_content,
    remote = "https://github.com/SFML/SFML",
)

git_repository(
    name = "rules_foreign_cc",
    branch = "master",
    remote = "https://github.com/bazelbuild/rules_foreign_cc",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()
