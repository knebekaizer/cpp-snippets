#!/bin/bash
#Declare array with 4 elements
ARRAY=( 01 02 03 04 )
#ARRAY=( 'Debian Linux' 'Redhat Linux' Ubuntu Linux )
# get number of elements in the array
LEN=${#ARRAY[@]}
echo "ARRAY: $ARRAY"
echo "LEN = $LEN"

case $1 in
	1|fatal|FATAL)  LogLevel=1; OPT_LogLevel='--log=::fatal';;
	2|error|ERROR)  LogLevel=2; OPT_LogLevel='--log=::error'
		;;
	3|warning|WARN|WARNING) LogLevel=3; OPT_LogLevel='--log=::warning'
		;;
	4|error|ERROR)          LogLevel=4; OPT_LogLevel='--log=::error'
		;;
	5|notice|NOTICE)        LogLevel=5; OPT_LogLevel='--log=::notice'
		;;
	6|info|information|INFO) LogLevel=6; OPT_LogLevel='--log=::information'
		;;
	7|debug|DEBUG)          LogLevel=7; OPT_LogLevel='--log=::debug'
		;;
	8|trace|TRACE)          LogLevel=8; OPT_LogLevel='--log=::trace'; 
		;;
	*)
		echo "Invalid log level: $2"
		exit 1
		;;
esac
echo "OPT_LogLevel = $OPT_LogLevel"


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
