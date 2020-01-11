cd /automata

#bazel clean --expunge
#bazel sync

bazel clean # this here only works because the directory we are in is the EXACT SAME NAME in the docker and native, otherwise bugs!!!
bazel build ...
bazel test ...