from ubuntu:20

run apt-get update
run apt-get install -qq icecc

ENTRYPOINT ["/usr/sbin/icecc-scheduler","-vvv"]