#!/usr/local/bin/bash 

echo $SHELL
echo "$0"

echo ".${0##*/}"
[ ".${0##*/}" = ".bash" ] && USE_BASH=true || USE_BASH=false

if $USE_BASH; then
	echo "Running bash!"
else
	echo "Running good old sh"
fi
