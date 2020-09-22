from ubuntu:20.04

run apt-get update
run apt-get install -qq icecc

ENTRYPOINT ["/usr/sbin/icecc-scheduler","-vvv","--listen","192.168.178.48"]