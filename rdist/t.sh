#!/bin/sh 


##-- Get configuration information
#base_name="`/usr/bin/basename $0`-service-tool"
#conf_file="${BASE_DIR}${base_name}.conf"
#. $conf_file || {
#  echo "Couldn't read configuration file for the service-tool, exiting."
#  exit 1
#}

echo $$

#----------------------------------------------------------
#-- 
#----------------------------------------------------------
finis() {
  ret=${1:-'0'}
  rm lock.1
  echo finis $ret
  exit $ret
}

atexit () {
  ret=${1:-'-42'}
  printf "atexit ${ret}\n" >&2
  exit $ret		# 'finis' to do cleanup
}

trap 'finis $?' 0
trap 'atexit 2' 2
trap 'atexit 15' 15 
trap 'atexit 30' 30 

TRAP=`trap`
printf "${TRAP}\n" >&2

touch lock.1

k=0
while [ $k -le 60 ]; do
    echo $k
    sleep 1
	k=$(($k+1))
#	[ $k -eq 30 ] && exit
done

