#!/bin/bash
#Declare array with 4 elements
ARRAY=( 01 02 03 04 )
#ARRAY=( 'Debian Linux' 'Redhat Linux' Ubuntu Linux )
# get number of elements in the array
LEN=${#ARRAY[@]}
echo "ARRAY: $ARRAY"
echo "LEN = $LEN"

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
