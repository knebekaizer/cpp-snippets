#!/bin/bash
#Declare array 
declare -a ARRAY
#Open file for reading to array
exec 11<array.sh
let count=0

while read LINE <&11; do
    ARRAY[$count]=$LINE
    ((count++))
done

echo Number of elements: ${#ARRAY[@]}
# echo array's content
#echo ${ARRAY[@]}
for (( i=0;i<${#ARRAY[@]};i++)); do
    echo ${ARRAY[${i}]}
done
echo "+++ done +++" 

# close file 
exec 11>&-
