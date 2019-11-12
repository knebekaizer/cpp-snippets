##
##!/bin/sh -T

echo $$
myPid=$$

finis() {
	ret=${1:-'0'}
	echo finis $ret
	exit $ret
}

atexit () {
	ret=${1:-'42'}
	printf >&2 "Terminated child $$: atexit ${ret}\n"
	exit $ret		# 'finis' to do cleanup
}

trap 'finis $?' EXIT
trap 'atexit 2' INT
trap 'atexit 15' TERM 
trap 'atexit 1' HUP 

echo ${trap}

while true; do
sleep 1
done
