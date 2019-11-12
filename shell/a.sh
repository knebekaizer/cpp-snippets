#!/bin/bash
#Declare array with 4 elements
ARRAY=( 01 02 03 04 )
#ARRAY=( 'Debian Linux' 'Redhat Linux' Ubuntu Linux )
# get number of elements in the array
LEN=${#ARRAY[@]}
echo "ARRAY: $ARRAY"
echo "LEN = $LEN"

foo() {
	OPT='foo'
	set OPT2 "foo"
	set $1  "foo"
}

OPT="main"
OPT1="main"
OPT2="main"
foo $OPT1
echo $OPT $OPT1 $OPT2

report() {
	echo "report: $*"
	return 15
#	echo "Running $# agents"
#	echo $*
}

count_files() {
	x=`[ -d $1 ] && ls $1 | wc -w`
	if [ -d $1 ]; then ls $1; fi | wc -w
	return $x
}

count_files $1; x=$?
echo "x = $x"
exit

report 3
echo $? "x = $x"

# echo each element in array 
# for loop
for (( i=0;i<$LEN;i++)); do
    echo ${ARRAY[${i}]}
done

for x in ${ARRAY[@]}; do
    echo $x
done

for (( k=-1; k<=4; k++)); do
    echo "k = $k"
done

exit
