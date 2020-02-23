#DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
DIR="$( dirname $(realpath $0) )"

docker build \
	--network=host \
	-t bazel-build \
	${DIR}


