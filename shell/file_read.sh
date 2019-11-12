#!/bin/bash

N=0
cat my.file | while read LINE ; do
	N=$((N+1))
	echo "Line $N = $LINE"
done

# or 

N=0
while read LINE ; do
	N=$((N+1))
	echo "Line $N = $LINE"
done < my.file


# or 

IFS=$'\n'
set $(cat my.file)
# Now the lines are stored in $1, $2, $3, ...
echo $1
echo $2
echo $3
echo $4

# or 

OLDIFS="$IFS"
IFS=
for entry in `cat foo`; do [ code here ]; done
IFS="$OLDIFS"

# cat file.lst |while read line; do newname=$(echo ${line}|sed 's/txt$/text/'); mv -v "${line}" "${newname}"; done