#!/bin/sh

for w in "$@"; do
	echo "$w"
done

#Words="${@:-`ls *`}"
Words="$@"
Words=${Words:="`ls *`"}

echo "\$# = $#"

for w in $Words; do
	echo "$w"
done

echo "*********************"

for w in "`ls *`"; do
	echo "$w"
done
