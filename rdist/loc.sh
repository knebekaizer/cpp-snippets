#!/usr/local/bin/bash 


##-- Get configuration information
#base_name="`/usr/bin/basename $0`-service-tool"
#conf_file="${BASE_DIR}${base_name}.conf"
#. $conf_file || {
#  echo "Couldn't read configuration file for the service-tool, exiting."
#  exit 1
#}

#echo $$

rHosts="dech001 dech002 dech043"
rAppName="rem.sh"
rPath="test"
rApp="$rPath/$rAppName"
rPidPath="$HOME/misc/snippets/rdist/run"

HOSTNAME=`hostname -s`

#----------------------------------------------------------
#-- 
#----------------------------------------------------------
atexit () {
  ret=${1:-'-42'}
  echo "atexit"
  rm lock.1
  kill "$p1"
  echo $ret
  exit $ret
}

# First param shall be log level, e.g. ERROR, INFO, etc
logmsg() {
    echo "[`date -j +'%Y-%m-%d %H:%M:%S'`] [$1> $2"
}

die() {
    logmsg ERROR $1
    exit -1
}

install() {
	echo "Installing:"
	for remotehost in $rHosts; do
		rsh $remotehost mkdir -p $rPath
	    rcp $rAppName $remotehost':'$rPath/ || die
	    echo "  $remotehost"
	done
}

run() {
#	echo $$ | rsh fryazino 'cat >>misc/snippets/rdist/run/'`hostname -s`
	echo "Running:"
	for remotehost in $rHosts; do
		param="$HOSTNAME $rPidPath/$remotehost"
	#	echo Running as rsh -n $remotehost $rApp $param
	#	echo `rsh -n $remotehost $rApp $param' >&/dev/null </dev/null & echo $!'` > $rPidPath/$remotehost
	
	#	rsh $remotehost $rApp $param &
	##	echo `rsh -n $remotehost $rApp $param' >&test/test.log </dev/null & echo $!'`
	
	rsh -n $remotehost "cd $rPath; umask 001;" 'mkdir -p arch && cat *.log >> arch/get.log && rm *.log;' ./$rAppName $param ' &> test.log < /dev/null &'
    touch $rPidPath/$remotehost
    echo "  $remotehost"
	done
}

report() {
	echo "Running $# agents"
	echo $*
}

status() {
	agents="`ls $rPidPath/`"
	n_agents=`echo $agents | wc -w`
	echo "Running $n_agents agents"
	for n in $agents ; do
		remotehost=$n
		# check if pid file is empty
		if [ -s $n ]; then
			logmsg WARNING "$remotehost: agent process not found"
		else 
			rpid="`cat $rPidPath/$n`"
			echo "remotehost = $remotehost"
			rsh $remotehost procstat $rpid
		fi
	done
}

stop() {
	for n in $rPidPath/*; do
		remotehost=`basename $n`
		rpid=`cat $n`
		echo trying to stop $rpid on $remotehost
		rc=`rsh $remotehost kill -15 $rpid; echo $?`; echo $rc
	done

}

#trap atexit 2 30 


##-- Figure out what we're doing
##for ((i=$OPTIND; i<=$# ; ++i)); do
#	case ${!i} in
#		-i ) install;;
#		-r ) run;;
#		-s ) status;;
#	esac
##done

if [ ".$1" == ".install" ]
then 
    install
elif [ ".$1" == ".run" ]
then
	run
elif [ ".$1" == ".stop" ]
then
	stop
else 
    status 
fi 


#touch lock.1



#sleep 60 & p1=$!
#sort -n $HOME/work/test/r.dat > 1.1 & p1=$!
#wait "$p1"

#atexit 0

