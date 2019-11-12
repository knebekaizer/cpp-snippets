#!/usr/local/bin/bash 


##-- Get configuration information
#base_name="`/usr/bin/basename $0`-service-tool"
#conf_file="${BASE_DIR}${base_name}.conf"
#. $conf_file || {
#  echo "Couldn't read configuration file for the service-tool, exiting."
#  exit 1
#}

parentHost=$1
thisHost=`hostname -s`
pidFile=$2

unlockCmd="rsh $1 rm $2" 
#unlockCmd="ssh $1 rm $pidFile" 
#unlockCmd="rm $pidFile"

#lockCmd="ssh $1 touch $2"
#remCmd="echo $$ > $pidFile"
#echo $remCmd
lockCmd="rsh $parentHost 'echo $$ > $pidFile'"
#lockCmd="echo $$ > $pidFile"

#echo $lockCmd
#$lockCmd

#exit

echo $$

#----------------------------------------------------------
#-- 
#----------------------------------------------------------
atexit () {
  ret=${1:-'-42'}
  logmsg DEBUG "atexit"
#  kill -15 `jobs -p` && echo "kill returns $?"
#  [ ".$p1" != ".0" ] && kill -15 "$p1" && echo "$p1 killed, $?"
  finis
  exit $ret
}

#----------------------------------------------------------
# First param shall be log level, e.g. ERROR, INFO, etc
#----------------------------------------------------------
logmsg() {
    echo -n "[`date -j +'%Y-%m-%d %H:%M:%S'`] [$1> "
    shift 
    echo $@
}

die() {
    logmsg ERROR $1
    exit -1
}
#----------------------------------------------------------

start() {
	rsh $parentHost "echo $$ > $pidFile"
}

log_pids() {
#	bgr=`jobs -p`
#	rsh $parentHost "echo -n \"$bgr $$\" > $pidFile"
	echo -n `jobs -p` $$ | rsh $parentHost "cat > $pidFile"
}

finis() {
	logmsg DEBUG finis $unlockCmd
	$unlockCmd
#	rsh fryazino rm $1
}

trap atexit 2 15 30 

log_pids

sleep 60 & p1=$!
log_pids
sleep 61 &
log_pids


#echo "sleep pid = " $p1
#sort -n $HOME/work/test/r.dat > 1.1 & p1=$!
wait
p1=0

finis 
