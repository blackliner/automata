DIR="$( dirname $( realpath $0 ) )"

docker run \
    --name bazel-build \
    --network=host \
    --rm \
    --user $UID:$GID \
    --workdir="/automata" \
    --volume="/etc/group:/etc/group:ro" \
    --volume="/etc/passwd:/etc/passwd:ro" \
    --volume="/etc/shadow:/etc/shadow:ro" \
    --volume="/etc/sudoers.d:/etc/sudoers.d:ro" \
    --volume="/var/run/docker.sock:/var/run/docker.sock:rw" \
    --volume="/tmp:/tmp:rw" \
    --volume="/home/$USER:/home/$USER:rw" \
    --volume="${DIR}/..:/automata:rw" \
    "$@" \
	bazel-build
