#!/bin/bash
#Read a text file word-by-word in BASH:
for WORD in `cat filename`
do
echo $WORD
done

#Read a text file line-by-line in BASH:
while read LINE
do
echo $LINE
done < filename

#Looping in BASH:
i=0
while [ $i -lt 10 ]
do
echo $i
# next
i=$(($i+1))
done

#Looping in BASH 2:
for (( i = 1; i <= 10; i++ )); do echo $i ; done
