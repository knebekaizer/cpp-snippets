#!/usr/local/bin/bash 

command=$*
echo $command
	
DELAY=5

retry=3
until [ $retry -eq 0 ] 
do
	retry=`expr $retry - 1`
	$command
	rc=$?
	if [ "$rc" -ne "0" ]
	then
		echo "rc = $rc; retries left: $retry"
		sleep $DELAY
	else
		retry=0
	fi	
done 
echo "Done: rc = $rc"

exit $rc
