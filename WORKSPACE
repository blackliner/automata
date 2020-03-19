workspace(
    name = "automata",
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")  #new:git_repository is needed for projects without a BUILD file ;-)

git_repository(
    name = "com_google_benchmark",
    branch = "master",
    remote = "https://github.com/google/benchmark",
    # commit = "367119482ff4abc3d73e4a109b410090fc281337",
    # shallow_since = "1575278956 +0000",
)

git_repository(
    name = "com_google_googletest",
    branch = "master",
    remote = "https://github.com/google/googletest",
    # commit = "367119482ff4abc3d73e4a109b410090fc281337",
    # shallow_since = "1575278956 +0000",
)

git_repository(
    name = "com_github_google_rules_install",
    remote = "https://github.com/google/bazel_rules_install.git",
    tag = "0.0.2",
)

load("@com_github_google_rules_install//:deps.bzl", "install_rules_dependencies")

install_rules_dependencies()

new_local_repository(
    name = "usr_local",
    build_file = "usr_local.BUILD",
    path = "/usr/local",
)

# needed packages
# sudo apt install libpng-dev
# sudo apt install libgl1-mesa-dev
