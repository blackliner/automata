from ubuntu:18.04

run apt-get update
run apt-get install -qq icecc

ENTRYPOINT ["/usr/sbin/iceccd","-vvv","-s","192.168.178.48"]