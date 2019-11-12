#!/bin/sh -T


echo $$
myPid=$$

cleanup() {
	rm lock.1
}

ps_children() {
	ps -w -o pgid,pid,time,args | sed -ne "/^$myPid / p" | sed -e 's/^[0-9]* *//'
}

finis() {
	ret=${1:-'0'}
	cleanup
	echo finis $ret
	exit $ret
}

atexit () {
	ret=${1:-'42'}
	printf >&2 "Terminated: atexit ${ret}\n"
	echo "jobid" 
	jobid
	echo
	
	echo "ps_children" 
	ps_children
	echo
	
	echo "jobs -l"
	jobs -l
	echo
	
	echo "jobs -p"
	jobs -p
	echo
	
	echo "jobs -s"
	jobs -s
	echo
	
	echo "Killing `jobs -s`"
	kill -$1 `jobs -s`
	
	echo "jobid" 
	jobid
	echo
	
	exit $ret		# 'finis' to do cleanup
}

trap 'finis $?' 0
trap 'atexit 2' 2
trap 'atexit 15' 15 
trap 'atexit 30' 30 

TRAP=`trap`
printf >&2 "${TRAP}\n"

touch lock.1

echo "sleep for 60 sec in background..."
sleep 62 &
sleep 61 &
sleep 60 &
echo "pid = $!"
echo "jobs -s = `jobs -s`"
echo "jobs -p = `jobs -p`"
echo "...continue"


echo "sleep for 10 sec..."
sleep 10
echo "...continue"
 
echo "Running nested bg task..."
( (  sleep 121 & echo "bg sleep $!"; );  sleep 120 & echo "bg sleep $!"; ) &  echo "bg sleep  $!"
echo "...continue"


echo "running long task..."
grep -R -E "something" / > /dev/null 2>&1
echo "...continue"

k=0
while [ $k -le 60 ]; do
    echo $k
    sleep 1
	k=$(($k+1))
#	[ $k -eq 30 ] && exit
done

