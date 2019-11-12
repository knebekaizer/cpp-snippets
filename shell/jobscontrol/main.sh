####
#!/bin/sh -T


echo $$
myPid=$$

if [ ! -z $BASH ]; then
	echo "Running Bash!"
	shopt -s huponexit
fi



cleanup() {
	rm lock.1
}

ps_children() {
	ps -w -o pgid,pid,time,args | sed -ne "/^$myPid / p" | sed -e 's/^[0-9]* *//'
}

finis() {
	ret=${1:-'0'}
	cleanup
	echo "finis $$ $ret"
	exit $ret
}

TrapList="1 2 15 30"
atexit () {
	ret=${1:-'42'}
	printf >&2 "Terminated main $$: atexit ${ret}\n"
	
	# remove trap handler to prevent infinite recursion when using 'sh -T'
	trap $1
	
#	echo "ps_children" 
#	ps_children
#	echo
	
#	echo "jobs -l"
#	jobs -l
#	echo
	
	echo "jobs -p"
	jobs -p
	echo
	
	echo "jobs -s"
	jobs -s
	echo
	
#	echo "Killing `jobs -s`"
#	kill -$1 `jobs -s`
	
#	echo "killing `jobs -p`"
#	kill -${1:-15} `jobs -p`
#	sleep 5
#	echo "killing `jobs -s`"
#	kill -${1:-15} `jobs -s`

# special convention is to propagate TERM instead on INT to background task
	local sig
	[ ".$1" = ".2" ] && sig=15 || sig=$1
	trap 'echo "Ignoring signal $sig on recursive trap"' $sig		# prevent infinite recursion in sh
	kill -$sig 0	
	trap "atexit $sig" $sig	
	
	exit $ret		# 'finis' to do cleanup
}

trap 'finis $?' 0
for x in $TrapList; do
	trap "atexit $x" $x
done
#trap 'atexit 1' HUP
#trap 'atexit 2' 2
#trap 'atexit 15' 15 
#trap 'atexit 30' 30 

TRAP=`trap`
printf >&2 "${TRAP}\n"

touch lock.1

./child.sh &
./child.sh &
./child.sh

wait

#while true; do
#    sleep 60
#done

